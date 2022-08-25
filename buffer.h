#pragma once 
#include "page.h"
#include "replacer.h"
#include "clock.h"
#include "LRU.h"
#include "MRU.h"
#include "file.h"
#include <unordered_map>
#include <iostream>


/*Estructura encargada de la abstraccion de como las paginas son administradas por el buffer*/
struct Frame
{
    Page    *page;          // Puntero a la pagina correspondiente
    int32_t dirty_bit;      // Indica si una pagina ha sido modificada
    int32_t pin_count;      // Cuantos tienen acceso a la pagina
};

class Buffer
{
    /*La clase buffer es la encargada de gestionar los datos almacenados en Disco (Gestion = Modificacion, Creacion y Eliminacion de Datos.)*/
public:
    Buffer(size_t pool_size, File *file);
    bool  FlushPageImpl (int32_t page_id);  // Actualiza los cambios realizados a una pagina por el buffer en el disco.
    Page* NewPageImpl   (int32_t *page_id); // Crea una nueva pagina en el buffer y en disco.
    Page* FetchPageImpl (int32_t page_id);  // Carga o hace el llamado a paginas que pueden o no estar dentro del Buffer.
    bool  DeletePageImpl(int32_t page_id);  // Elimina pagina alojada en el Buffer.
    bool  UnpinPageImpl (int32_t page_id, bool is_dirty); // Reduce los procesos que estan utilizando una pagina.
    void  FlushAllPagesImpl();              // Actualiza todos los cambios de todas las paginas en el buffer.
    ~Buffer();
    std::vector<Frame> buffer_pool; // Vector de las paginas en el buffer.
private:
    size_t pool_size_;              // Tamanio del buffer    
    Replacer *replacer_;            // Clase abstracta (polimorfismo)
    std::list<int32_t> free_list_;  // Lista de indices de frames libres
    std::unordered_map<int32_t, int32_t> page_table_; // Mapa de <idPage , idFrame>
    File *file_;                    // Puntero a File o Gestor de Disco, con el que se realizan los llamados a las paginas
    
};

//Constructor con tamanio de buffer_pool y puntero al File
Buffer::Buffer(size_t pool_size, File *file) : pool_size_(pool_size), file_(file)
{
    buffer_pool = std::vector<Frame>(pool_size);
    //Inicializamos el replacer con el algoritmo de remplazamiento elegido 
    replacer_ = new Clock(pool_size);
    for (int32_t i = 0; i < pool_size; i++)
    {
        //Inicializamos las listas en 0
        free_list_.emplace_back(static_cast<int>(i));
        buffer_pool[i].dirty_bit = 0;
        buffer_pool[i].pin_count = 0;
    }
}
//Obtener pagina solicitada por id
Page* Buffer::FetchPageImpl(int32_t page_id) 
{
    // 1. Buscar en la page_table la página solicitada (P).
    // 1.1 Si P existe, la fija y la devuelve inmediatamente.
    // 1.2 Si P no existe, encuentra una página de reemplazo (R) de la lista libre o del replacer.
    // 2. Si R está dirty, escribirla de nuevo en el disco.
    // 3. Eliminar R de la page_table e insertar P.
    // 4. Actualizar los metadatos de P, leer el contenido de la página desde el disco, y luego devolver un puntero a P.


    //Si la pagina se encuentra ya en el buffer.
    if(page_table_.find(page_id)!= page_table_.end())
    {
        //Actualiza metadata de la pagina en el Buffer
        int32_t target = page_table_[page_id];      // devuelve frame_id
        replacer_->Pin(target);
        buffer_pool[target].pin_count++;
        //Retorna el puntero la pagina encontrada
        return buffer_pool[target].page;
    }
    //En caso la pagina solicitada, NO se encuentre en el buffer
    else
    {
        //Seteamos un entero que reciba el indice a utilizar
        int32_t target;
        if(!free_list_.empty()){
        //Si existen espacios disponibles
            target = free_list_.front();
            free_list_.pop_front();
            //Target se setea como el espacio encontrado
            replacer_->Pin(target);
            //Actualizamos los contenidos de la page_table y el buffer_pool
            page_table_.insert({page_id, target});
            buffer_pool[target].pin_count++;
            buffer_pool[target].page = file_->readPage(page_id);
            buffer_pool[target].dirty_bit = 0;
            //Retornamos el puntero a la pagina requerida 
            return buffer_pool[target].page;
        }
        else
        {//Sino, utilizamos el algoritmo de remplazamiento
            if(!replacer_->Victim(&target)) 
            {
                //std::cout << "NULL\n";
                return nullptr;
            }
            int32_t target_page_id = buffer_pool[target].page->page_number(); // Page_id de la pagina que se eliminara del buffer
            std::cout << "target_page_id: " << target_page_id << "\n"; 
            
            if(buffer_pool[target].dirty_bit)
            {
                if(!FlushPageImpl(target_page_id)) 
                {
                    return nullptr;
                }
            }
            replacer_->Pin(target);
            buffer_pool[target].pin_count++;
            //Actualizamos los contenidos de la page_table y el buffer_pool
            page_table_.erase(target_page_id);          // Borra la pagina del buffer
            page_table_.insert({page_id, target});
            buffer_pool[target].page = file_->readPage(page_id);  // Lee nueva pagina del file
            buffer_pool[target].dirty_bit = 0;
            //Retornamos el puntero a la pagina requerida 
            return buffer_pool[target].page;
        }
        
    }
    return nullptr;
}

bool Buffer::UnpinPageImpl(int32_t page_id, bool is_dirty)
{
    // Si la pagina  no se encuentra en el buffer, no se realiza el proceso
    if(page_table_.find(page_id) == page_table_.end())
    {
        return true;
    }
    // Se localiza el frame que contiene la pagina
    int32_t target = page_table_[page_id];
    // Verificamos el marcado del pincount
    if(buffer_pool[target].pin_count <= 0)
    {
        return false;
    }
    else 
    {
        // Se actualiza la metadata del frame correspondiente.
        buffer_pool[target].pin_count--;
        buffer_pool[target].dirty_bit |= is_dirty;
        if(buffer_pool[target].pin_count == 0)
        {
            replacer_->Unpin(target);
        }
        return true;
    }
}

bool Buffer::FlushPageImpl(int32_t page_id) {
    // Comprobamos que se trata de una peticion a una pagina valida
    if(page_id == -1){return false;}

    // Comprobamos su existencia en el Buffer.
    if(page_table_.find(page_id) == page_table_.end()){
        return false;
    }
    else {
        // Ubicamos el frame donde se ubica la pagina
        int32_t target = page_table_[page_id];
        // Si no se encuentra modificada, terminamos la ejecucion
        if(!buffer_pool[target].dirty_bit){
            return true;
        }
        //Caso contrario, la actualizamos en el disco, y retornamos.
        file_->write_page(page_id, buffer_pool[target].page);
        buffer_pool[target].dirty_bit = 0;
        return true;
    }
}

Page *Buffer::NewPageImpl(int32_t *page_id) {
    // 1. Si todas las páginas del bufferpool están fijadas == pinned, devuelve nullptr.
    // 2. Escoge una página víctima P de la lista libre o del replacer. Siempre se elige primero de la lista libre.
    // 3. Actualizar los metadatos de P, poner a cero la memoria y añadir P a la page_table.
    // 4. Establecer el parámetro de salida del ID de la página. Devuelve un puntero a P.
    if(!free_list_.empty()){                        // Si existe un frame libre, lo toma y asigna.
        int32_t free_frame = free_list_.front();    // Indice de frame libre
        free_list_.pop_front();                     // Elimina indice
        *page_id = file_->allocatePage();           // Page_id guarda id nueva pagina
        page_table_[*page_id] = free_frame;         // Guarda frame
        replacer_->Pin(free_frame);                 // Marca la pagina como "En Uso"
        buffer_pool[free_frame].page        = new Page(*page_id); // Guarda nueva pagina en el frame
        buffer_pool[free_frame].pin_count   = 1;                  // Uno en uso
        buffer_pool[free_frame].dirty_bit   = false;              // No esta modificado

        //std::cout << "Numero asig: " << *page_id << "\n";
        return buffer_pool[free_frame].page;        // Devuelve puntero a la pagina
    }
    else{                       // Si no hay frames disponibles 
        int32_t victim;         
        if(!replacer_->Victim(&victim)) { // Algoritmo de remplazamiento
            return nullptr;
        }

        if(buffer_pool[victim].dirty_bit){  // Si ha sido modificado
            if(!FlushPageImpl(buffer_pool[victim].page->page_number())) { // Lo manda al file
                return nullptr;
            }
        }
        // Coloca la pagina nueva en el buffer
        int32_t victim_page_id = buffer_pool[victim].page->page_number();
        page_table_.erase(victim_page_id);      // Borra 
        *page_id = file_->allocatePage();       // Nuevo page_id
        page_table_[*page_id] = victim;         // Guarda frame_id
        replacer_->Pin(victim);                 // Setea la pagina como "En Uso"
        buffer_pool[victim].page      = new Page(*page_id); // Nueva pagina
        buffer_pool[victim].pin_count = 1;      
        buffer_pool[victim].dirty_bit = false; 

        return buffer_pool[victim].page;    // Devuelve puntero a la pagina
    }
}

bool Buffer::DeletePageImpl(int32_t page_id) {
    // 1. Buscar en la page_table la página solicitada (P).
    // 1. Si P no existe, devuelve true.
    // 2. Si P existe, pero tiene un número de pines distinto de cero, devuelve false. 
    //    Alguien está usando la página.
    // 3. En caso contrario, P puede ser eliminado. 
    //    Elimina P de la page_table, restablece sus metadatos y la devuelve a la lista de libres.
    if(page_table_.find(page_id) == page_table_.end()){
        return false;
    }
    else{
        int32_t target = page_table_[page_id]; // devuelve frame_id
        if(buffer_pool[target].pin_count != 0) {    // si esta siendo usado no elimina
            return false;
        }else{ 
            page_table_.erase(page_id);     // elimina
            buffer_pool[target].page = nullptr;     // vacio
            buffer_pool[target].dirty_bit = false;  // 
            free_list_.push_back(target);   // indice del frame libre
            return true;
        }
    }
    return false;
}

void Buffer::FlushAllPagesImpl(){
    for(size_t i = 0; i < pool_size_; ++i){
        if(buffer_pool[i].dirty_bit){
            if(!FlushPageImpl(buffer_pool[i].page->page_number()))
            {
            }
        }
    }
}

Buffer::~Buffer(){
    delete replacer_;
}
