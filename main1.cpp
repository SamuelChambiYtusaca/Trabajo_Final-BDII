#include <bits/stdc++.h>
#include "page.h"

using namespace std;

int main()
{
    vector<string> data = {"Este", "es", "un", "conjunto", "de", "registros"};
    vector<int> slots;
    int k=1;
    Page new_page(k);

    cout << "Creacion de nueva pagina e insercion de primeros registros:\n";
    for(int i=0 ; i<data.size() ; ++i)
    {
        int rid;
        rid = new_page.insert_record(data[i]);
        slots.push_back(rid);
    }
    for(auto it = new_page.begin() ; it!=new_page.end() ; ++it )
        cout    <<"Pagina: "<<it->page_number<<" Slot: "<<it->slot_number<<" Record_id: "<<it->record_ID <<' '<<it->record <<'\n';
    cout    <<'\n';

    cout << "Obtencion de registro mediante id: \n";
    cout<<  new_page.get_record(slots[1])<<'\n';

    new_page.delete_record(slots[1]);
    cout << "Obtencion de registro mediante id despues de la eliminacion: \n";
    cout << new_page.get_record(slots[1]);
    cout << endl;

    cout << "Mostando pagina actualizada:\n";
    for(auto it = new_page.begin() ; it!=new_page.end() ; ++it )
        cout    <<"Pagina: "<<it->page_number<<" Slot: "<<it->slot_number<<" Record_id: "<<it->record_ID <<' '<<it->record <<'\n';
    cout    <<'\n';

    cout<< "Slots libres: "<< new_page.get_num_free_slot() <<'\n';

    int rid = new_page.insert_record("Nuevo registro");
    cout << "Mostrando registro recien ingresado:\n";
    cout<< new_page.get_record(rid)<<'\n';
    cout<< "Slots libres: "<< new_page.get_num_free_slot()<<'\n';

    cout << "Mostando pagina actualizada:\n";
    for(auto it = new_page.begin() ; it!=new_page.end() ; ++it )
        cout    <<"Pagina: "<<it->page_number<<" Slot: "<<it->slot_number<<" Record_id: "<<it->record_ID <<' '<<it->record <<'\n';
    cout    <<'\n';
    cout<< "Demostrando pagina sin espacio disponible:\n";
    rid = new_page.insert_record("Nuevo registro");

    cout << "Actualizacion de registro en una pagina:\n";
    new_page.updated_record(slots[2], " Actualizacion de Registro");
    cout<<new_page.get_record(slots[2])<<'\n';

    cout << "Mostando pagina actualizada:\n";
    for(auto it = new_page.begin() ; it!=new_page.end() ; ++it )
        cout    <<"Pagina: "<<it->page_number<<" Slot: "<<it->slot_number<<" Record_id: "<<it->record_ID <<' '<<it->record <<'\n';
    cout    <<'\n';
}
