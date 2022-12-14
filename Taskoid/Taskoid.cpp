/*
*                           INFORMACION DEL PROYECTO
* 
*   Nombre del proyecto >> TASKOID
*   Desarrolladores >> 
*       - JOSÉ VALENTINO MASÍAS CASTILLO
*       - PERCY JESÚS DEZA PAZ
* 
* 
*           LINKS DE LAS SOLUCIONES A PROBLEMAS PRESENTADOS EN EL DESARROLLO: 
*
* - https://learn.microsoft.com/es-es/azure/mysql/single-server/connect-cpp
* - https://www.sololearn.com/compiler-playground/c39N9RN6b4Md/
* - https://stackoverflow.com/questions/15435994/how-do-i-open-an-exe-from-another-c-exe
*/
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <fstream>
//LIBRERIAS PROPIAS DE MYSQL CONNECTOR C++
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

using namespace std;
using namespace sql;

const string server = "localhost"; // servidor
const string username = "root"; // usuario
const string password = "josvalPassword"; // contraseña

struct User
{
    string names;
    string lastnames;
    string username;
    string password;
};

User* user = NULL;

User* create_user(string, string, string, string);
void insert_user_data(User*&, string, string, string, string);
void menu_login_register();
void user_login();
void user_register();
void menu_app();

void menu_user();
void menu_modify_user();

void view_all_tasks();
void create_new_task();

void view_pending_tasks();
void view_finished_tasks();
void view_normal_tasks();
void view_important_tasks();
void view_urgent_tasks();
void export_tasks();

Driver* driver;
Connection* con;
Statement* stmt;
PreparedStatement* pstmt;
ResultSet* result;

int main()
{
    try
    {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
        cout << "Conectado a la base de datos!\n" << endl;
    }
    catch(SQLException e)
    {
        cout << "No se pudo conectar al servidor. \nMensaje de error: " << e.what() << endl;
        system("pause");
        // retornar una salida del programa
        exit(1);
    }

    // Seleccionando la base de datos "taskoid"
    con->setSchema("taskoid");

    menu_login_register();
    return 0;
}

User* create_user(string names, string lastnames, string username, string password)
{
    User* new_user = new User();
    new_user->names = names;
    new_user->lastnames = lastnames;
    new_user->username = username;
    new_user->password = password;

    return new_user;
}

void insert_user_data(User*& user, string names, string lastnames, string username, string password)
{
    if (user == NULL)
    {
        User* new_user = create_user(names, lastnames, username, password); // variable temporal
        user = new_user;
    }
    else
    {
        user = NULL;
        insert_user_data(user, names, lastnames, username, password);
    }
}

void menu_login_register()
{
    int opc;

    system("cls"); // clear screen 
    system("color cf");
    
    do
    {
        cout << "\t\t\t:: IDENTIFICATE ::" << endl << endl;
        cout << "\t[1] Iniciar Sesion" << endl;
        cout << "\t[2] Registrarme" << endl;
        cout << "\t[3] Salir" << endl;
        cout << "\n\t>> ";
        cin >> opc;

        switch (opc)
        {
        case 1:
            user_login();
            break;
        case 2:
            user_register();
            break;
        }
        system("cls");
    } while (opc != 3);

}

void user_login()
{
    string username, password;
    cout << "\n\t:: Nombre de usuario >> ";
    cin >> username;
    cout << "\t:: Clave >> ";
    cin >> password;
    
    pstmt = con->prepareStatement("SELECT * FROM users WHERE username = ?");
    pstmt->setString(1, username);
    result = pstmt->executeQuery();

    while (result->next())
    {
        if (result->getString(4) != username)
        {
            //...
            return;
        }
        if(result->getString(4) == username)
        {
            if (result->getString(5) != password)
            {
                //....
                return;
            }
            if (result->getString(5) == password)
            {
                cout << "Inicio de sesion exitoso" << endl;
                insert_user_data(user, result->getString(2), result->getString(3), username, password);
                cout << "\n\n";
                system("pause");
                menu_app();
            }
        }
    }
}

void user_register()
{
    int counter = 0;
    string names, lastnames, username, password;
    cout << "\n\t:: Nombre >> ";
    cin >> names;
    cout << "\t:: Apellido >> ";
    cin >> lastnames;
    cout << "\t:: Nombre de usuario >> ";
    cin >> username;
    cout << "\t:: Clave >> ";
    cin >> password;

    //Verificando si el usuario ya se encuentra registrado
    pstmt = con->prepareStatement("SELECT username FROM users WHERE username = ?");
    pstmt->setString(1, username);
    result = pstmt->executeQuery();

    while (result->next())
    {
        if (result->getString(1) == username)
        {
            counter++;
        }
    }

    if (counter != 0)
    {
        //Ya se encuentra registrado (no apto para registrar)
        return;
    }
    if (counter == 0)
    {
        //No se encuentra registrado (apto para registrar)
        pstmt = con->prepareStatement("INSERT INTO users (names, lastnames, username, password) VALUES (?, ?, ?, ?)");
        pstmt->setString(1, names);
        pstmt->setString(2, lastnames);
        pstmt->setString(3, username);
        pstmt->setString(4, password);

        try
        {
            pstmt->execute();
            cout << "\nRegistrado con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo registrar\n :: Error >> " << e.what() << endl;
            system("pause");
        }
    }
}

void menu_app()
{
    int opc;

    system("cls");

    do
    {
        cout << "\t\t\t:: TASKOID ::" << endl << endl;
        cout << "\t[1] Mi usuario" << endl;
        cout << "\t[2] Ver  todas mis tareas" << endl;
        cout << "\t[3] Crear nueva tarea" << endl;
        cout << "\t[4] Ver mis tareas pendientes" << endl;
        cout << "\t[5] Ver mis tareas finalizadas" << endl;
        cout << "\t[6] Ver mis tareas normales" << endl;
        cout << "\t[7] Ver mis tareas importantes" << endl;
        cout << "\t[8] Ver mis tareas urgentes" << endl;
        cout << "\t[9] Exportar tareas" << endl;
        cout << "\t[0] Salir" << endl;
        cout << "\n\t>> ";
        cin >> opc;

        switch (opc)
        {
        case 1:
            menu_user();
            break;
        case 2:
            view_all_tasks();
            break;
        case 3:
            create_new_task();
            break;
        case 4:
            view_pending_tasks();
            break;
        case 5:
            view_finished_tasks();
            break;
        case 6:
            view_normal_tasks();
            break;
        case 7:
            view_important_tasks();
            break;
        case 8:
            view_urgent_tasks();
            break;
        case 9:
            export_tasks();
            break;
        }
        system("cls");
    } while (opc != 0);

}

void menu_user()
{
    int opc;

    system("cls");
    do
    {
        cout << "\t\t\t:: MI USUARIO ::" << endl << endl;
        cout << "\t[1] Ver Informacion" << endl;
        cout << "\t[2] Actualizar informacion" << endl;
        cout << "\t[3] Salir" << endl;
        cout << "\n\t>> ";
        cin >> opc;

        switch (opc)
        {
        case 1:
            system("cls");
            cout << "\t\t\t:: MI INFORMACION ::" << endl;
            cout << "\tNombre : " << user->names << endl;
            cout << "\tApellido : " << user->lastnames << endl;
            cout << "\tUsuario : " << user->username << endl << endl;
            system("pause");
            break;
        case 2:
            menu_modify_user();
            break;
        }
        system("cls");
    } while (opc != 3);
}

void menu_modify_user()
{
    int opc;
    string newName, newLastname;

    system("cls");
    do
    {
        cout << "\t\t\t:: MI USUARIO ::" << endl << endl;
        cout << "\t[1] Actualizar nombre" << endl;
        cout << "\t[2] Actualizar apellido" << endl;
        cout << "\t[3] Salir" << endl;
        cout << "\n\t>> ";
        cin >> opc;

        switch (opc)
        {
        case 1:
            system("cls");
            cout << "Ingrese el nuevo nombre" << endl << "> ";
            cin >> newName;
            pstmt = con->prepareStatement("UPDATE users SET names = ? WHERE names = ?");
            pstmt->setString(1, newName);
            pstmt->setString(2, user->names);
            try
            {
                pstmt->execute();
                cout << "\nNombre actualizado con exito!" << endl << endl;
                user->names = newName;
                system("pause");
            }
            catch (SQLException e)
            {
                cout << "No se pudo actualizar el nombre\n :: Error >> " << e.what() << endl;
                system("pause");
            }
            break;
        case 2:
            system("cls");
            cout << "Ingrese el nuevo apellido" << endl << "> ";
            cin >> newLastname;
            pstmt = con->prepareStatement("UPDATE users SET lastnames = ? WHERE lastnames = ?");
            pstmt->setString(1, newLastname);
            pstmt->setString(2, user->lastnames);
            result = pstmt->executeQuery();
            try
            {
                pstmt->execute();
                cout << "\nApellido actualizado con exito!" << endl << endl;
                user->lastnames = newLastname;
                system("pause");
            }
            catch (SQLException e)
            {
                cout << "No se pudo actualizar el apellido\n :: Error >> " << e.what() << endl;
                system("pause");
            }
            break;
        }
        system("cls");
    } while (opc != 3);
}

void view_all_tasks()
{
    int opc, id, counter = 1;
    system("cls");
    cout << "\t\t\t:: TODAS MIS TAREAS ::" << endl << endl;
    pstmt = con->prepareStatement("SELECT * FROM tasks WHERE created_by = ?");
    pstmt->setString(1, user->username);
    result = pstmt->executeQuery();

    while (result->next())
    {
        cout << "NRO. " << counter << endl;
        cout << "ID >> " << result->getInt(1) << endl;
        cout << "Titulo >> " << result->getString(2) << endl;
        cout << "Descripcion >> " << result->getString(3) << endl;
        cout << "Tipo >> " << result->getString(4) << endl;
        cout << "Estado >> " << result->getString(5) << endl << endl;
        counter++;
    }
    cout << "\n\n\t:: [1] Marcar una tarea como finalizada" << endl;
    cout << "\t:: [2] Marcar una tarea como pendiente" << endl;
    cout << "\t:: [3] Eliminar tarea" << endl;
    cout << "\t:: [4] Salir" << endl;
    cout << ">> ";
    cin >> opc;

    switch (opc)
    {
    case 1:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("UPDATE tasks SET status = 'Finalizado' WHERE id = ? AND created_by = ?");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea marcada como finalizada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo marcar como finalizado\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 2:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("UPDATE tasks SET status = 'Pendiente' WHERE id = ? AND created_by = ? AND status = 'Finalizado'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea marcada como pendiente con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo marcar como pendiente\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 3:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("DELETE FROM tasks WHERE id = ? AND created_by = ? AND status = 'Pendiente'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea eliminada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo eliminar la tarea\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 4:
        break;
    default:
        break;
    }
}

void create_new_task()
{
    int opc;
    string title, description, type;
    system("cls");
    cout << "\t\t\t:: CREAR NUEVA TAREA ::" << endl;
    cout << "\t:: Titulo >> ";
    cin >> title;
    cout << "\t:: Descripcion >> ";
    cin >> description;
    cout << "\t:: Tipo " << endl;
    cout << "\t\t[1] Normal" << endl;
    cout << "\t\t[2] Importante" << endl;
    cout << "\t\t[3] Urgente" << endl;
    cout << "\t >> ";
    cin >> opc;
    switch (opc)
    {
    case 1:
        type = "Normal";
        break;
    case 2:
        type = "Importante";
        break;
    case 3:
        type = "Urgente";
        break;
    default:
        cout << "\n\n\t\t\t:: OPCION INVALIDA! ::" << endl << endl;
        system("pause");
        create_new_task();
        return;
        break;
    }
    pstmt = con->prepareStatement("INSERT INTO tasks (title, description, type, status, created_by) values (?, ?, ?, ?, ?)");
    pstmt->setString(1, title);
    pstmt->setString(2, description);
    pstmt->setString(3, type);
    pstmt->setString(4, "Pendiente");
    pstmt->setString(5, user->username);

    try
    {
        pstmt->execute();
        cout << "\nTarea agregada con exito!" << endl << endl;
        system("pause");
    }
    catch (SQLException e)
    {
        cout << "No se pudo agregar la tarea\n :: Error >> " << e.what() << endl;
        system("pause");
    }
}

void view_pending_tasks()
{
    int opc, id, counter = 1;
    system("cls");
    cout << "\t\t\t:: TODAS MIS TAREAS PENDIENTES ::" << endl << endl;
    pstmt = con->prepareStatement("SELECT * FROM tasks WHERE created_by = ? AND status = 'Pendiente'");
    pstmt->setString(1, user->username);
    result = pstmt->executeQuery();

    while (result->next())
    {
        cout << "NRO. " << counter << endl;
        cout << "ID >> " << result->getInt(1) << endl;
        cout << "Titulo >> " << result->getString(2) << endl;
        cout << "Descripcion >> " << result->getString(3) << endl;
        cout << "Tipo >> " << result->getString(4) << endl;
        cout << "Estado >> " << result->getString(5) << endl << endl;
        counter++;
    }
    cout << "\n\n\t:: [1] Marcar una tarea como finalizada" << endl;
    cout << "\t:: [2] Eliminar tarea" << endl;
    cout << "\t:: [3] Salir" << endl;
    cout << ">> ";
    cin >> opc;

    switch (opc)
    {
    case 1:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("UPDATE tasks SET status = 'Finalizado' WHERE id = ? AND created_by = ? AND status = 'Pendiente'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea marcada como finalizada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo marcar como finalizado\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 2:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("DELETE FROM tasks WHERE id = ? AND created_by = ? AND status = 'Pendiente'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea eliminada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo eliminar la tarea\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 3:
        break;
    default:
        break;
    }
}

void view_finished_tasks()
{
    int opc, id, counter = 1;
    system("cls");
    cout << "\t\t\t:: TODAS MIS TAREAS FINALIZADAS ::" << endl << endl;
    pstmt = con->prepareStatement("SELECT * FROM tasks WHERE created_by = ? AND status = 'Finalizado'");
    pstmt->setString(1, user->username);
    result = pstmt->executeQuery();

    while (result->next())
    {
        cout << "NRO. " << counter << endl;
        cout << "ID >> " << result->getInt(1) << endl;
        cout << "Titulo >> " << result->getString(2) << endl;
        cout << "Descripcion >> " << result->getString(3) << endl;
        cout << "Tipo >> " << result->getString(4) << endl;
        cout << "Estado >> " << result->getString(5) << endl << endl;
        counter++;
    }

    cout << "\n\n\t:: [1] Marcar una tarea como pendiente" << endl;
    cout << "\t:: [2] Eliminar tarea" << endl;
    cout << "\t:: [3] Salir" << endl;
    cout << ">> ";
    cin >> opc;

    switch (opc)
    {
    case 1:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("UPDATE tasks SET status = 'Pendiente' WHERE id = ? AND created_by = ? AND status = 'Finalizado'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea marcada como pendiente con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo marcar como pendiente\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 2:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("DELETE FROM tasks WHERE id = ? AND created_by = ? AND status = 'Finalizado'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea eliminada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo eliminar la tarea\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 3:
        break;
    default:
        break;
    }
}

void view_normal_tasks()
{
    int opc, id, counter = 1;
    system("cls");
    cout << "\t\t\t:: TODAS MIS TAREAS ::" << endl << endl;
    pstmt = con->prepareStatement("SELECT * FROM tasks WHERE created_by = ? AND type = 'Normal'");
    pstmt->setString(1, user->username);
    result = pstmt->executeQuery();

    while (result->next())
    {
        cout << "NRO. " << counter << endl;
        cout << "ID >> " << result->getInt(1) << endl;
        cout << "Titulo >> " << result->getString(2) << endl;
        cout << "Descripcion >> " << result->getString(3) << endl;
        cout << "Tipo >> " << result->getString(4) << endl;
        cout << "Estado >> " << result->getString(5) << endl << endl;
        counter++;
    }
    cout << "\n\n\t:: [1] Marcar una tarea como finalizada" << endl;
    cout << "\t:: [2] Marcar una tarea como pendiente" << endl;
    cout << "\t:: [3] Eliminar tarea" << endl;
    cout << "\t:: [4] Salir" << endl;
    cout << ">> ";
    cin >> opc;

    switch (opc)
    {
    case 1:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("UPDATE tasks SET status = 'Finalizado' WHERE id = ? AND created_by = ? AND type = 'Normal'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea marcada como finalizada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo marcar como finalizado\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 2:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("UPDATE tasks SET status = 'Pendiente' WHERE id = ? AND created_by = ? AND status = 'Finalizado' AND type = 'Normal'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea marcada como pendiente con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo marcar como pendiente\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 3:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("DELETE FROM tasks WHERE id = ? AND created_by = ? AND status = 'Pendiente' AND type = 'Normal'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea eliminada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo eliminar la tarea\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 4:
        break;
    default:
        break;
    }
}

void view_important_tasks()
{
    int opc, id, counter = 1;
    system("cls");
    cout << "\t\t\t:: TODAS MIS TAREAS ::" << endl << endl;
    pstmt = con->prepareStatement("SELECT * FROM tasks WHERE created_by = ? AND type = 'Importante'");
    pstmt->setString(1, user->username);
    result = pstmt->executeQuery();

    while (result->next())
    {
        cout << "NRO. " << counter << endl;
        cout << "ID >> " << result->getInt(1) << endl;
        cout << "Titulo >> " << result->getString(2) << endl;
        cout << "Descripcion >> " << result->getString(3) << endl;
        cout << "Tipo >> " << result->getString(4) << endl;
        cout << "Estado >> " << result->getString(5) << endl << endl;
        counter++;
    }
    cout << "\n\n\t:: [1] Marcar una tarea como finalizada" << endl;
    cout << "\t:: [2] Marcar una tarea como pendiente" << endl;
    cout << "\t:: [3] Eliminar tarea" << endl;
    cout << "\t:: [4] Salir" << endl;
    cout << ">> ";
    cin >> opc;

    switch (opc)
    {
    case 1:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("UPDATE tasks SET status = 'Finalizado' WHERE id = ? AND created_by = ? AND type = 'Importante'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea marcada como finalizada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo marcar como finalizado\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 2:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("UPDATE tasks SET status = 'Pendiente' WHERE id = ? AND created_by = ? AND status = 'Finalizado' AND type = 'Importante'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea marcada como pendiente con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo marcar como pendiente\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 3:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("DELETE FROM tasks WHERE id = ? AND created_by = ? AND status = 'Pendiente' AND type = 'Importante'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea eliminada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo eliminar la tarea\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 4:
        break;
    default:
        break;
    }
}

void view_urgent_tasks()
{
    int opc, id, counter = 1;
    system("cls");
    cout << "\t\t\t:: TODAS MIS TAREAS ::" << endl << endl;
    pstmt = con->prepareStatement("SELECT * FROM tasks WHERE created_by = ? AND type = 'Urgente'");
    pstmt->setString(1, user->username);
    result = pstmt->executeQuery();

    while (result->next())
    {
        cout << "NRO. " << counter << endl;
        cout << "ID >> " << result->getInt(1) << endl;
        cout << "Titulo >> " << result->getString(2) << endl;
        cout << "Descripcion >> " << result->getString(3) << endl;
        cout << "Tipo >> " << result->getString(4) << endl;
        cout << "Estado >> " << result->getString(5) << endl << endl;
        counter++;
    }
    cout << "\n\n\t:: [1] Marcar una tarea como finalizada" << endl;
    cout << "\t:: [2] Marcar una tarea como pendiente" << endl;
    cout << "\t:: [3] Eliminar tarea" << endl;
    cout << "\t:: [4] Salir" << endl;
    cout << ">> ";
    cin >> opc;

    switch (opc)
    {
    case 1:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("UPDATE tasks SET status = 'Finalizado' WHERE id = ? AND created_by = ? AND type = 'Urgente'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea marcada como finalizada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo marcar como finalizado\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 2:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("UPDATE tasks SET status = 'Pendiente' WHERE id = ? AND created_by = ? AND status = 'Finalizado' AND type = 'Urgente'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea marcada como pendiente con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo marcar como pendiente\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 3:
        cout << "\t:: Ingresar ID de la tarea: ";
        cin >> id;
        pstmt = con->prepareStatement("DELETE FROM tasks WHERE id = ? AND created_by = ? AND status = 'Pendiente' AND type = 'Urgente'");
        pstmt->setInt(1, id);
        pstmt->setString(2, user->username);
        try
        {
            pstmt->execute();
            cout << "\n:: Tarea eliminada con exito!" << endl << endl;
            system("pause");
        }
        catch (SQLException e)
        {
            cout << "No se pudo eliminar la tarea\n :: Error >> " << e.what() << endl;
            system("pause");
        }
        break;
    case 4:
        break;
    default:
        break;
    }
}

void export_tasks()
{
    system("cls");
    cout << "\t\t\t:: TODAS MIS TAREAS ::" << endl << endl;
    pstmt = con->prepareStatement("SELECT * FROM tasks WHERE created_by = ?");
    pstmt->setString(1, user->username);
    result = pstmt->executeQuery();

    ofstream dir("TAREAS EXPORTADAS.txt");
    if (!dir)
    {
        cout << "Error al abrir 'TAREAS EXPORTADAS.txt'" << endl << endl;
        system("pause");
        return;
    }

    while (result->next())
    {
        dir << "ID >> " <<  result->getInt(1) << endl;
        dir << "TITULO >> " <<  result->getString(2) << endl;
        dir << "DESCRIPCION >> " <<  result->getString(3) << endl;
        dir << "TIPO >> " <<  result->getString(4) << endl;
        dir << "ESTADO >> " <<  result->getString(5) << endl;
        dir << "CREADO POR >> " <<  result->getString(6) << endl << endl << endl;
    }

    cout << "\t:: Tareas exportadas con exito!" << endl << endl;

    system("start notepad.exe './TAREAS EXPORTADAS.txt'");
    system("pause");

}
