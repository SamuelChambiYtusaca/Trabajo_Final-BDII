#ifndef PAGE_H
#define PAGE_H
#include <vector>
#include <iostream>
#include <string>

struct PageHeader       // Metadata de la pagina
{
    int size;                   // Tamanio de la pagina
    int num_free_slots;         // Numero slots libres
    int num_slots;              // Numero slots usados
    int current_page_number;    // id pagina
    int next_page_number;       // id sig pagina
    bool used;                  // Indica si la pagina esta en uso
    const PageHeader& operator=(const PageHeader& p_h)
    {
        if( &p_h != this )
        {
            size = p_h.size;
            num_free_slots = p_h.num_free_slots;
            num_slots = p_h.num_slots;
            current_page_number = p_h.current_page_number;
            next_page_number = p_h.next_page_number;
            used = 0;
        }
        return *this;
    }
};

struct Record           // Estructura del Registro, en conjunto con su metadata
{
    std::string record;     //  Registro
    int record_ID;          //  Id de registro
    int page_number;        //  id_pagina
    int slot_number;        //  Posicion que ocupa
    Record(std::string record_,int record_ID_, int page_number_, int slot_number_)
    {
        record = record_;
        record_ID = record_ID_;
        page_number = page_number_;
        slot_number = slot_number_;
    }
};

class Page
{
    private:
        PageHeader header;
        std::vector<Record> page;

        int get_available_slot();                                       // Slot disponible para insertar registro
        int getslot(int record_ID);                                     // Obtiene slot de un registro
        bool validate_recordID(int record_ID);                          // Verifica el record_id

    public:
        Page(int current_page_number_);
        Page(const Page& new_page);
        //Operaciones con registros
        int insert_record(std::string record_data);                     // Insercion
        std::string get_record(int record_ID);                          // Obtencion
        void updated_record(int record_ID, std::string record_data);    // Actualizacion
        void delete_record(int record_ID);                              // Eliminacion
        bool has_space_for_record();                                    // Comprobante de espacio

        bool is_used(){ return header.used; }                           // Indica si la pagina esta en uso
        int get_num_free_slot(){ return header.num_free_slots; }        // Cantidad slots libres
        int get_size(){ return header.size; }                           // Cantidad maxima slots
        int get_num_free_slots() { return header.num_free_slots; }                       
        int get_num_slots() { return header.num_slots; }
        int page_number(){ return header.current_page_number; }         // Page_id
        int next_page_number(){ return header.next_page_number; }       // Sig page_id
        void clear();

        void set_page_number(int new_page_number)
        {
            header.current_page_number = new_page_number;
            header.next_page_number = header.current_page_number + 1;
        }
        void set_next_page_number(int new_next_page_number)
        {
            header.next_page_number = new_next_page_number;
        }

        const Page& operator=(const Page& page_)
        {
            // std::cout<<"usando esto = '\n";
            if( &page_ != this )
            {
                header = page_.header;
                page = page_.page;
            }
            return *this;
        }

        std::vector<Record>::iterator begin(){ return page.begin(); }
        std::vector<Record>::iterator end(){ return page.end(); }
};

Page::Page(int current_page_number_)
{
    int n = 6;
    header.size = n;
    header.num_free_slots = n;
    header.num_slots = 0;
    header.current_page_number = current_page_number_;
    header.next_page_number = current_page_number_ + 1;
    header.used = 0;
}

Page::Page(const Page& new_page)
{
    header = new_page.header;
    page = new_page.page;
}

int Page::get_available_slot()
{
    int i = page.size();
    for(int j=0 ; j<page.size() ; ++j)
        if(page[j].record == " ")
            return j;
    return i;
}

int Page::getslot(int record_ID)
{
    return page[record_ID].slot_number;
}

bool Page::validate_recordID(int record_ID)
{
    if( record_ID >= get_size() )
    {
        std::cout << " 1 Requerimiento dirigido a registro invalido."
                  << " Registro {pagina=" << page_number()
                  << ", slot=" << getslot(record_ID);
        return false;
    }
    if(is_used())
    {
        std::cout << " 1 Requerimiento dirigido a registro invalido."
                  << " Registro {pagina=" << page_number()
                  << ", slot=" << getslot(record_ID);
        return false;
    }
    return true;
}

int Page::insert_record(std::string record_data)
{
    if( !has_space_for_record() ) 
    {
        std::cout << "Espacio insuficiente en la pagina " << page_number()
                  << " para almacenar el registro.\n"; 
        return -1;
    }
    int slot = get_available_slot();
    Record new_record(record_data, slot, page_number(), slot);
    if(slot < page.size())
        page[slot] = new_record;
    else
        page.push_back(new_record);
    header.num_free_slots--;
    header.num_slots++;
    return slot;
}

std::string Page::get_record(int record_ID)
{
    int slot = getslot(record_ID);
    return page[slot].record;
}

void Page::updated_record(int record_ID, std::string record_data)
{
    if( !validate_recordID(record_ID) )
        return;
    int free_space_after_delete = get_num_free_slot();
    int slot = getslot(record_ID);
    page[slot].record += record_data;
}

void Page::delete_record(int record_ID)
{
    if( !validate_recordID(record_ID) )
        return;
    int slot = getslot(record_ID);
    page[slot].record = " ";
    header.num_free_slots++;
    header.num_slots--;
}

bool Page::has_space_for_record()
{
    if(get_num_free_slot() > 0)
        return true;
    return false;
}

void Page::clear()
{
    header.num_free_slots = header.size;
    page.clear();
    header.current_page_number = 0;
    header.next_page_number = 0;
    header.num_slots = 0;
    header.used = 0;
}

#endif
