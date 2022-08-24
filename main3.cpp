#include <bits/stdc++.h>
#include "file.h"
#include "page.h"

using namespace std;

int main()
{
    string file_name = "test2.txt";
    File new_file(file_name);
    new_file.open('|');

    file_name = "test.txt";
    new_file.set_filename(file_name);
    new_file.open('|');

    cout<<'\n';
    cout<< "Paginas en el file: " << new_file.get_num_pages() <<'\n';
    for(auto it = new_file.begin() ; it!=new_file.end() ; ++it)
    {
        cout<<"Registros en la pagina "<< it->page_number()<<": "<< it->get_num_slots() <<'\n';
    }
    cout<<'\n';

    // Lee pagina del file con iteradores   
    for(auto it = new_file.begin() ; it!=new_file.end() ; ++it)
    {
        cout<<  "Pagina: "<<it->page_number()<<'\n';
        for(auto it2 = it->begin() ; it2 != it->end() ; ++it2)
            cout <<"slot "<<it2->slot_number<<" Record_id "<<it2->record_ID<<" "<<it2->record <<'\n';
        cout<<'\n';
    }
    cout<<'\n';

    // Modificando file y guardando
    cout<<"MODIFICANDO FILE Y GUARDANDO\n";
    int page_id = new_file.allocatePage();
    vector<string> data = {"Nuevo", "dataset", "de", "registros", "almacenados"};
    vector<int> slots;
    vector<int> pages;
    Page new_page(page_id);
    for(int i=0 ; i<data.size() ; ++i)
    {
        int rid = new_page.insert_record(data[i]);
        slots.push_back(rid);
    }
    new_file.write_page(page_id, &new_page);

    cout<<'\n';
    cout <<  "Paginas en el file: "<<new_file.get_num_pages()<<'\n';
    for(auto it = new_file.begin() ; it!=new_file.end() ; ++it)
    {
        cout<< "Registros en la pagina "<< it->page_number()<<": "<<it->get_num_slots()<<'\n';
    }
    cout<<'\n';

    // Lee pagina del file con iteradores   
    for(auto it = new_file.begin() ; it!=new_file.end() ; ++it)
    {
        cout << "Pagina: "<<it->page_number()<<'\n';
        for(auto it2 = it->begin() ; it2 != it->end() ; ++it2)
            cout <<"Slot: "<<it2->slot_number<<" Record_id: "<<it2->record_ID<<" "<<it2->record <<'\n';
        cout<<'\n';
    }
    cout<<'\n';
    
    // Guardando en otro file
    cout<<"GUARDANDO DATA EN OTRO FILE\n";
    string new_file_name = "data.txt";
    new_file.set_filename(new_file_name);

    cout<<'\n';
    cout<<"Paginas en el file: "<<new_file.get_num_pages()<<'\n';
    for(auto it = new_file.begin() ; it!=new_file.end() ; ++it)
    {
        cout << "Registros en la pagina "<<it->page_number()<<": "<<it->get_num_slots()<<'\n';
    }
    cout<<'\n';

    // Lee pagina del file con iteradores   
    for(auto it = new_file.begin() ; it!=new_file.end() ; ++it)
    {
        cout << "Pagina: "<<it->page_number()<<'\n';
        for(auto it2 = it->begin() ; it2 != it->end() ; ++it2)
            cout<<"Slot: "<<it2->slot_number<<" Record_id: "<<it2->record_ID<<" "<<it2->record <<'\n';
        cout<<'\n';
    }
    cout<<'\n';
}