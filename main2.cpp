#include <bits/stdc++.h>
#include "file.h"
#include "page.h"

using namespace std;

int main()
{
    vector<string> data = {"Este", "es", "un", "conjunto", "de", "registros"};
    vector<string> data2 = {"Este", "es", "un", "segundo", "conjunto", "dataset"};
    vector<string> data3 = {"Tercer", "conjunto", "dataset", "mediante", "clase", "File"};
    vector<int> slots1;
    vector<int> slots2;
    vector<int> slots3;
    vector<int> pages;
    string file_name = "test.txt";

    // fase insercion
    cout<<"FASE DE INSERCION\n";
    File new_file(file_name);
    // primera pagina
    int page_id = new_file.allocatePage();      // Crea pagina
    pages.push_back(page_id);                   // Reserva page_id
    Page new_page(page_id);
    for(int i=0 ; i<data.size() ; ++i)
    {
        int rid = new_page.insert_record(data[i]);
        slots1.push_back(rid);                  // Almacenando slots de Pagina 1
    }
    new_file.write_page(page_id, &new_page);    // Escribe la pagina en disco

    // Pagina 2
    page_id = new_file.allocatePage();          // Crea pagina
    pages.push_back(page_id);                   // Reserva page_id
    new_page.clear();
    new_page.set_page_number(page_id);
    for(int i=0 ; i<data2.size() ; ++i)
    {
        int rid = new_page.insert_record(data2[i]);
        slots2.push_back(rid);                  // Almacenando slots de Pagina 1
    }
    new_file.write_page(page_id, &new_page);    // Escribe la pagina en disco

    // Pagina 3
    page_id = new_file.allocatePage();          // Repetimos el proceso
    pages.push_back(page_id);                   
    new_page.clear();
    new_page.set_page_number(page_id);
    for(int i=0 ; i<data3.size() ; ++i)
    {
        int rid = new_page.insert_record(data3[i]);
        slots3.push_back(rid);                  
    }
    new_file.write_page(page_id, &new_page);    

    // Lee paginas del fie con iteradores
    for(auto it = new_file.begin() ; it!=new_file.end() ; ++it)
    {
        cout<<"pagina: "<<it->page_number()<<'\n';
        for(auto it2 = it->begin() ; it2 != it->end() ; ++it2)
            cout<< it2->record <<' ';
        cout<<'\n';
    }
    cout<<'\n';

    // Lee paginas del file con id's
    for(int k=0 ; k<pages.size() ; ++k)
    {
        Page* current_page = new_file.readPage(pages[k]);
        cout<<"Pagina "<<current_page->page_number()<<'\n';
        cout<<"Registros guardados:\n";
        for(auto it = current_page->begin() ; it != current_page->end() ; ++it)
            cout<< it->record <<' ';
        cout<<'\n';
    }
    
    cout<<'\n';
    cout<<  "Paginas en el file: "<<    new_file.get_num_pages()<<'\n';
    for(auto it = new_file.begin() ; it!=new_file.end() ; ++it)
    {
        cout<<"Registros en la pagina "<<it->page_number()<<": "<<it->get_num_slots()<<'\n';
    }

}