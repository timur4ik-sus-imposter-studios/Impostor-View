#include "local.h"

std::vector<std::vector<std::wstring>> local_matrix = {
    { L"File", L"Файл" },   
    {
        L"Impostor View 1.1.0\n(C) 2026, Naumkin Timur\n(C) 2026, Sus Imposter Studios\n\neasy ico viewer, my personal project\n\nlicense: MIT License",
        L"Impostor View 1.1.0\n(C) 2026, Наумкин Тимур\n(C) 2026, Sus Imposter Studios\n\nпростое средство просмотра .ico\n\nЛицензия: MIT License"
    },   
    { L"Save as windows icon file", L"Сохранить как файл иконки windows" },   
    { L"Open file", L"Открыть файл" },   
    { L"Edit", L"Правка" },   
    { L"Quit from program", L"Выход из программы" },      
    { L"plus", L"приблизить" },   
    { L"minus", L"отдалить" },   
    { L"File not selected", L"Файл не выбран" },   
    { L"Select lang", L"Выбрать язык" },
    { L"Information", L"Справка"},
    { L"About a program", L"О программе"}
};
Lang lang = EN;

void SelectLang() {
    if (lang == EN)
    {
        lang = RU;
        return;
    }
    lang = EN;
}
