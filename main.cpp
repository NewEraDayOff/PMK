#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <map>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <cmath>

enum task_type {MAIN, OS, CASHIER, NEW_CHECK, ADD} type;

// Статистика чеков после закрытия смены
class CheckStat
{
    // Число сформированных чеков с оплатой картой
    int cardCheckAmount = 0;
    // Число сформированных чеков с оплатой наличными
    int cashCheckAmount = 0;
    // Общая сумма оплаты картой
    float cardSummary = 0.0f;
    // Общая сумма оплаты наличными
    float cashSummary = 0.0f;
public:
    // После оформления нового чека обновляются статистические переменные
    void newCashCheck(float price) {this->cashCheckAmount++; this->cashSummary += price;};
    void newCardCheck(float price) {this->cardCheckAmount++; this->cardSummary += price;};

    // Геттеры числа сформированных чеков с оплатой картой и оплатой наличными соответственно
    int getcardCheckAmount() const {return this->cardCheckAmount;};
    int getcashCheckAmount() const {return this->cashCheckAmount;};

    // Геттеры общей суммы оплаты картой и суммы оплаты наличными соответственно
    float getCardSummary() const {return this->cardSummary;};
    float getCashSummary() const {return this->cashSummary;};

    // Очистка статистики
    void clear()
    {
        this->cardCheckAmount = 0;
        this->cashCheckAmount = 0;
        this->cardSummary = 0.0f;
        this->cashSummary = 0.0f;
    };
};

// Товарные позиции
class Product
{
    // Наименование товара
    const std::wstring name = L"";
    // Цена товара
    const float price = 0.0f;
    // Штрих-код товара
    const std::wstring barcode = L"";
    // Количество товаров
    int amount = 0;
public:
    // Конструктор
    Product(std::wstring n, int a, std::wstring b, float p) : name(n), amount(a), barcode(b), price(p) {};
    
    // Геттер наименования товара
    const std::wstring& getName() const {return this->name;};

    // Геттер цены товара
    const float getPrice() const {return this->price;};

    // Геттер Штрих-кода товара
    const std::wstring& getBarcode() const {return this->barcode;};

    // Геттер количества товара
    const int getAmount() const {return this->amount;};

    // Метод уменьшения количества товара
    void decreaseAmount(int number) {this->amount -= number;};
};

// Касса
class CashReg
{
    // Наличность
    float cash = 3000.0f;
    // Кассир
    std::wstring cashier = L"";
public:
    // Геттер наличных кассы
    float getCash() const {return this->cash;};

    // Метод уменьшения наличных в кассе
    void decreaseCash(float number) {this->cash -= number;};
    // Метод увеличения наличных в кассе
    void increaseCash(float number) {this->cash += number;};

    // Сеттер кассира
    void setCashier(const std::wstring &s) {this->cashier = s;};
    // Геттер кассира
    const std::wstring& getCashier() const {return this->cashier;};
    // Проверка существования кассира
    bool noCashier() {return this->cashier.empty();};
};

// Запрос
class Task
{
    // Данный запрос
    const task_type current_task;
    // Запрос выхода
    bool exit = false;
protected:
    // Касса
    CashReg* cashReg = nullptr;
    // Следующая задача
    task_type next_task = MAIN;
    // Описание допустимых запросов
    std::vector<std::wstring> helpCommands = {
        L"h\nhelp\nПомощь с вводом команд",
        L"e\nexit\nВыход из программы"};
    
    // Вывод вспомогательных команд
    void printHelpCommands(std::vector<std::wstring> helpCommands) const;
public:
    // Конструктор
    Task(CashReg* cr, task_type tt) : cashReg(cr), current_task(tt), next_task(tt) {};

    // Геттер описания допустимых запросов
    const std::vector<std::wstring>& getHelpCommands() const {return this->helpCommands;};

    // Геттер следующего запроса
    const task_type getNextTaskType() const {return this->next_task;};

    // Геттер данного запроса
    const task_type getCurrentTaskType() const {return this->current_task;}

    // Сеттер следующего запроса
    void setNextTaskType(task_type tt) {this->next_task = tt;};

    // Выполнение запроса
    virtual bool completeTask(const std::wstring& s);

    // Сеттер кассы
    void setCashReg(CashReg* cr) {this->cashReg = cr;};

    // Геттер проверки запроса выхода
    bool isExit() const {return this->exit;};
};


void Task::printHelpCommands(std::vector<std::wstring> helpCommands) const
{
    for (int i = 0; i < helpCommands.size(); i++)
    {
        std::wstringstream ss(helpCommands[i]);
        std::wstring substr;
        std::getline(ss, substr, L'\n');
        std::wcout << std::left << std::setw(5) << substr;
        std::getline(ss, substr, L'\n');
        std::wcout << std::left << std::setw(15) << substr;
        std::getline(ss, substr, L'\n');
        std::wcout << substr;
        std::wcout << L"\n";
    }
}

// Выполнение запроса
bool Task::completeTask(const std::wstring& s)
{
    // Если запрос на выход
    if (!s.compare(L"e") || !s.compare(L"exit"))
    {
        this->exit = true;
        return false;
    }
    // Если запрос помощи
    if (!s.compare(L"h") || !s.compare(L"help"))
    {
        // Выводим всю вспомогательную информацию
        this->printHelpCommands(this->helpCommands);
        return true;
    }
    return false;
};

// Основной запрос
class MainTask : public Task
{
public:
    // Конструктор
    MainTask(CashReg* cr, task_type tt);

    // Выполнение запроса
    bool completeTask(const std::wstring &s) override;
};

// Конструктор
MainTask::MainTask(CashReg* cr, task_type tt = MAIN):Task(cr, tt)
{
    helpCommands.push_back(L"os\nopening shift\nКаждый кассир приходя на работу открывает смену");
};

// Выполнение запроса
bool MainTask::completeTask(const std::wstring &s)
{
    // Проверка соответствия запроса базовым
    if (Task::completeTask(s)) return true;
    // Если запрос открыть смену
    if (!s.compare(L"os") || !s.compare(L"opening shift"))
    {
        this->next_task = OS;
        std::wcout << L"Введите имя кассира: ";
        return true;
    }
    return false;
};

// Запрос открытия смены
class OSTask : public Task
{
public:
    // Конструктор
    OSTask(CashReg* cr, task_type tt = OS):Task(cr, tt){};

    // Выполнение запроса
    bool completeTask(const std::wstring& s) override;
};

// Выполнение запроса
bool OSTask::completeTask(const std::wstring &s)
{
    this->cashReg->setCashier(s);
    this->next_task = CASHIER;
    std::wcout << L"Добрый день, " << this->cashReg->getCashier() << L"! Если не знаете что вводить, введите help." << L"\n";
    return true;
};

// Запрос смены
class CashierTask : public Task
{
    // Статистика по смене
    CheckStat* stat;
public:
    // Конструктор
    CashierTask(CashReg* cr, task_type tt);
    
    // Сеттер статистики по смене
    void setCheckStat(CheckStat* cs) {this->stat = cs;};

    // Выполнение запроса
    bool completeTask(const std::wstring& s) override;
};

// Конструктор
CashierTask::CashierTask(CashReg* cr, task_type tt = CASHIER) : Task(cr, tt)
{
    this->helpCommands.push_back(L"cs\nclosing shift\nКаждый кассир уходя с работы закрывает смену");
    this->helpCommands.push_back(L"nc\nnew check\nНачать формировать новый чек");
    this->helpCommands.push_back(L"ac\nadd cash\nВ денежный ящик можно внести деньги");
};

// Выполнение запроса
bool CashierTask::completeTask(const std::wstring &s)
{
    // Проверка базовых запросов
    if (Task::completeTask(s)) return true;
    // Запрос ввода наличных в денежный ящик
    if (!s.compare(L"ac") || !s.compare(L"add cash"))
    {
        std::wcout << L"Сколько денег вы хотите внести?" << L"\n";
        std::wstring task;
        while (true)
        {
            std::getline(std::wcin, task);
            float cash;
            try
            {
                cash = std::stof(task);
            }
            catch(const std::exception& e)
            {
                std::wcout << L"Введено неверное значение" << L"\n";
                continue;
            }
            if (cash > 0.00f)
            {
                this->cashReg->increaseCash(cash);
                std::wcout << L"Средства внесены. На счету в денежном ящике "
                          << std::fixed << std::setprecision(2) << this->cashReg->getCash()
                          << L"руб. Если не знаете что вводить, введите help." << L"\n";
                return true;
            }
            std::wcout << L"Введено неверное значение" << L"\n";
        }
    }

    // Запроса закрытия смены
    if (!s.compare(L"cs") || !s.compare(L"closing shift"))
    {
        this->next_task = MAIN;
        std::wcout << L"Статистка за смену:" << L"\n";
        std::wcout << L"Произведено " << this->stat->getcardCheckAmount()
                  << L" оплат картой на общую сумму в " << std::setprecision(2) << this->stat->getCardSummary() << L" руб." << L"\n";
        std::wcout << L"Произведено " << this->stat->getcashCheckAmount()
                  << L" оплат наличными на общую сумму в " << std::setprecision(2) << this->stat->getCashSummary() << L" руб." << L"\n";
        std::fixed;
        std::wcout << L"В кассе осталось " << std::fixed << std::setprecision(2) << this->cashReg->getCash() << L" руб." << L"\n";
        std::wcout << this->cashReg->getCashier() << L" закрывает смену. Если не знаете что вводить, введите help." << L"\n";
        this->cashReg->setCashier(L"");
        this->stat->clear();
        return true;
    }

    // Запрос формирования нового чека
    if (!s.compare(L"nc") || !s.compare(L"new check"))
    {
        std::wcout << L"Начинаем формирования чека. Если не знаете что вводить, введите help." << L"\n";
        this->next_task = NEW_CHECK;
        return true;
    }
    return false;
};

// Запрос чека
class CheckTask : public Task
{
    // Индексы товаров в чеке
    std::vector<int> productIndex;
    // Количество товаров в чеке
    std::vector<int> productAmount;
    // Товарная база
    std::vector<Product*> products;
    // Статистика по смене
    CheckStat* stat;

    // Вывод списка товаров по странице
    void loadProductBase(int page) const;

    // Вывод списка товаров по странице в поиске
    void loadProductBaseFromSearch(int page, std::vector<int> indexv) const;

    // Геттер количества оставшихся товаров по индексу
    int getProductAmountByIndex(int index) const;

    // Печать чека
    void printCheck(float price, float payment, bool by_card);

    // Обновление товарной базы
    void updateProductFile();

    // Запрос вспомогательной информации для списка товаров
    void listHelpTask() const;

    // Запрос вспомогательной информации для списка найденных товаров
    void searchHelpTask() const;

    // Запрос отмены формирования чека
    bool cancelTask();

    // Добавление товара в чек
    void addProductToCheck(int index);

    // Поиск товаров по наименованию и штрих-коду
    std::vector<int> findAllIndexesByNameOrBarcode(const std::wstring& name);

public:
    // Конструктор
    CheckTask(CashReg* cr, std::vector<Product*> &pv, task_type tt);
    
    // Сеттер статистики по смене
    void setCheckStat(CheckStat* cs) {this->stat = cs;};

    // Выполнение запроса
    bool completeTask(const std::wstring& s) override;
};

// Конструктор
CheckTask::CheckTask(CashReg* cr, std::vector<Product*> &pv, task_type tt = NEW_CHECK) : Task(cr, tt), products(pv)
{
    this->helpCommands.push_back(L"l\nlist\nСписок товаров");
    this->helpCommands.push_back(L"s\nsearch\nПоиск товаров");
    this->helpCommands.push_back(L"cc\nclose check\nЗакрытие чека");
    this->helpCommands.push_back(L"c\ncancel\nОтмена чека");
};

// Вывод списка товаров по странице
void CheckTask::loadProductBase(int page) const
{
    std::wcout << std::fixed;
    std::wcout << std::left << std::setw(5) << L"#"
              << std::left << std::setw(20) << L"Product name"
              << std::left << std::setw(15) << L"Barcode"
              << std::left << std::setw(15) << L"Price"
              << L"Amount" << L"\n";

    int startIndex = page * 5;
    for (int i = startIndex; i < startIndex + 5 && i < this->products.size(); i++)
    {
        auto p = this->products[i];
        std::wcout << std::left << std::setw(5) << i + 1
                  << std::left << std::setw(20) << p->getName()
                  << std::left << std::setw(15) << p->getBarcode()
                  << std::left << std::setw(15) << std::setprecision(2) << p->getPrice()
                  << this->getProductAmountByIndex(i) << L"\n";
    }
};

// Вывод списка товаров по странице в поиске
void CheckTask::loadProductBaseFromSearch(int page, std::vector<int> indexv) const
{
    std::wcout << std::fixed;
    std::wcout << std::left << std::setw(5) << L"#"
              << std::left << std::setw(20) << L"Product name"
              << std::left << std::setw(15) << L"Barcode"
              << std::left << std::setw(15) << L"Price"
              << L"Amount" << L"\n";
    
    int startIndex = page * 5;
    for (int i = startIndex; i < startIndex + 5 && i < indexv.size(); i++)
    {
        auto p = this->products[indexv[i]];
        std::wcout << std::left << std::setw(5) << indexv[i] + 1
                  << std::left << std::setw(20) << p->getName()
                  << std::left << std::setw(15) << p->getBarcode()
                  << std::left << std::setw(15) << std::setprecision(2) << p->getPrice()
                  << this->getProductAmountByIndex(indexv[i]) << L"\n";
    }
}

// Геттер количества оставшихся товаров по индексу
int CheckTask::getProductAmountByIndex(int index) const
{
    for (int i = 0; i < this->productIndex.size(); i++)
    {
        if (this->productIndex[i] == index)
        {
            return this->products[index]->getAmount() - this->productAmount[i];
        }
    }
    return this->products[index]->getAmount();
}

// Печать чека
void CheckTask::printCheck(float price, float payment, bool by_card)
{
    std::wcout << std::fixed;
    std::wcout << std::left << std::setw(5) << L"#"
              << std::left << std::setw(20) << L"Product name"
              << std::left << std::setw(10) << L"Amount"
              << std::left << std::setw(5) << L"x"
              << std::left << std::setw(10) << L"Price"
              << std::left << std::setw(5) << L"="
              << L"Summary" << L"\n";
    for (int i = 0; i < this->productIndex.size(); i++)
    {
        auto p = this->products[this->productIndex[i]];
        std::wcout << std::left << std::setw(5) << i + 1
                  << std::left << std::setw(20) << p->getName()
                  << std::left << std::setw(10) << this->productAmount[i]
                  << std::left << std::setw(5) << L"x"
                  << std::left << std::setw(10) << p->getPrice()
                  << std::left << std::setw(5) << L"="
                  << this->productAmount[i] * p->getPrice() << L"\n";
    }
    std::wcout << L"Итог: " << price << L" руб." << L"\n";
    std::wcout << L"Способ оплаты: " << (by_card ? L"картой" : L"наличными") << L"\n";
    std::wcout << L"Сумма оплаты: " << payment << L" руб." << L"\n";
    std::wcout << L"Сумма сдачи: " << std::setprecision(2) << payment - price << L" руб." << L"\n";
};

// Обновление товарной базы
void CheckTask::updateProductFile()
{
    for (int i = 0; i < this->productIndex.size(); i++)
    {
        this->products[this->productIndex[i]]->decreaseAmount(this->productAmount[i]);
    }
    this->productIndex.clear();
    this->productAmount.clear();

    std::wfstream file;
    file.open(L"prodcd.csv", std::ios::out | std::ios::trunc);
    for (auto p : this->products)
    {
        file << p->getName() << L","
             << p->getAmount() << L","
             << p->getBarcode() << L","
             << p->getPrice() << L"\n";
    }
    file.close();
}

// Запрос вспомогательной информации для списка товаров
void CheckTask::listHelpTask() const
{
    std::vector<std::wstring> helpCommands = 
    {
        L"Введите номер из списка товаров, указанный в левой части таблицы.\n\n",
        L"h\nhelp\nПомощь с вводом команд",
        L"c\ncancel\nОтмена добавления товара",
        L"l\nlist\nВывод списка товаров",
        L"n\nnext\nСледующая страница",
        L"p\nprev\nПредыдущая страница"
    };
    this->printHelpCommands(helpCommands);
};

// Запрос вспомогательной информации для списка найденных товаров
void CheckTask::searchHelpTask() const
{
    std::vector<std::wstring> helpCommands =
    {
        L"Введите номер из списка товаров, указанный в левой части таблицы.\n\n",
        L"h\nhelp\nПомощь с вводом команд",
        L"c\ncancel\nОтмена поиска товара",
        L"l\nlist\nВывод списка найденных товаров",
        L"n\nnext\nСледующая страница",
        L"p\nprev\nПредыдущая страница"
    };
    this->printHelpCommands(helpCommands);
};

// Запрос отмены формирования чека
bool CheckTask::cancelTask()
{
    std::wstring task;
    std::wcout << L"Вы уверены, что хотите отменить формирование чека [y/n]?" << L"\n";
    // Запрос отмены формирования чека
    std::getline(std::wcin, task);
    // Если ответ положительный
    if (!task.compare(L"y") || !task.compare(L"Y"))
    {
        this->productAmount.clear();
        this->productIndex.clear();
        std::wcout << L"Формирование чека прервано. Если не знаете что вводить, введите help." << L"\n";
        this->next_task = CASHIER;
        return true;
    }

    // Если ответ отрицательный
    if (!task.compare(L"n") || !task.compare(L"N"))
    {
        std::wcout << L"Формирование чека продолжается. Если не знаете что вводить, введите help." << L"\n";
        return true;
    }

    return false;
};

// Добавление товара в чек
void CheckTask::addProductToCheck(int index)
{
    bool productInCheck = false;
    for (int i = 0; i < this->productIndex.size(); i++)
    {
        // Если продукт уже в чеке, увеличиваем количество
        if (this->productIndex[i] == index - 1)
        {
            this->productAmount[i]++;
            productInCheck = true;
            break;
        }
    }

    // Если товар вне чека, добавляем товар в чек
    if (!productInCheck)
    {
        this->productIndex.push_back(index - 1);
        this->productAmount.push_back(1);
    }
    std::wcout << L"Товар " << this->products[index - 1]->getName() 
               << L" (" << this->products[index - 1]->getBarcode()
               << L") добавлен в чек!" << L"\n";
}

// Поиск по наименованию и штрих-коду
std::vector<int> CheckTask::findAllIndexesByNameOrBarcode(const std::wstring& name)
{
    std::vector<int> indexv;
    for (int i = 0; i < this->products.size(); i++)
    {
        auto pos = this->products[i]->getName().find(name);

        // Если найдено совпадение по имени, запоминаем индекс
        if (pos != std::wstring::npos)
        {
            bool exist = false;
            for (int j = 0; j < indexv.size(); j++)
            {
                // Если индекс уже появлялся, пропускаем его
                if (indexv[j] == i) exist = true;
            }
            if (exist) continue;
            indexv.push_back(i);
        }
        pos = this->products[i]->getBarcode().find(name);

        // Если найдено совпадение по штрих-коду
        if (pos != std::wstring::npos)
        {
            bool exist = false;
            for (int j = 0; j < indexv.size(); j++)
            {
                // Если индекс уже появлялся, пропускаем его
                if (indexv[j] == i) exist = true;
            }
            if (exist) continue;
            indexv.push_back(i);
        }
    }
    return indexv;
};

// Выполнение запроса
bool CheckTask::completeTask(const std::wstring& s)
{
    // Проверка базовых запросов
    if (Task::completeTask(s)) return true;

    // Если запрос отмены формирования чека
    if (!s.compare(L"c") || !s.compare(L"cancel"))
    {
        while (true)
        {
            if (this->cancelTask()) 
            {
                return true;
            }
        }
    }

    // Если запрос формирования списка товаров
    if (!s.compare(L"l") || !s.compare(L"list"))
    {
        int page = 0;
        this->loadProductBase(page);
        
        std::wcout << L"Введите номер из списка товаров. Если не знаете что вводить, введите help." << L"\n";
        std::wstring task;
        while (true)
        {
            // Запрос номера товара или иного запроса
            std::getline(std::wcin, task);

            // Если запрос помощи, выводится вся полезная информация
            if (!task.compare(L"h") || !task.compare(L"help"))
            {
                this->listHelpTask();
                continue;
            }
            
            // Если запрос отмены вывода списка, закрываем список товаров
            if (!task.compare(L"c") || !task.compare(L"cancel"))
            {
                std::wcout << L"Список товаров закрыт. Если не знаете что вводить, введите help." << L"\n";
                break;
            }
            int psz = this->products.size();
            int sz = psz % 5 ? psz - psz % 5 + 5 : psz;

            // Если запрос вывода списка товаров, снова выводим его
            if (!task.compare(L"l") || !task.compare(L"list"))
            {
                this->loadProductBase(page % (sz / 5));
                continue;
            }

            // Если запрос следующей страницы, листаем список товаров
            if (!task.compare(L"n") || !task.compare(L"next"))
            {
                page = page == sz / 5 - 1 ? 0 : page + 1;
                this->loadProductBase(page % (sz / 5));
                continue;
            }

            // Если запрос предыдущей страницы, листаем список товаров
            if (!task.compare(L"p") || !task.compare(L"prev"))
            {
                page = page ? page - 1 : sz / 5 - 1;
                this->loadProductBase(page % (sz / 5));
                continue;
            }
            int index;
            try
            {
                // Принимаем номер товара
                index = std::stoi(task);
            }
            catch(const std::exception& e)
            {
                std::wcout << L"Номер введён неверно. Если не знаете что вводить, введите help." << L"\n";
                continue;
            }

            // Если номер товара в рамках товарного списка
            if (index > 0 && index <= psz)
            {
                // Если товар кончился
                if (this->getProductAmountByIndex(index - 1) < 1)
                {
                    std::wcout << L"Товар закончился, невозможно добавить в чек" << L"\n";
                    continue;
                }

                this->addProductToCheck(index);
                continue;
            }
            std::wcout << L"Номер введён неверно. Если не знаете что вводить, введите help." << L"\n";
        }
        return true;
    }
    
    // Если запрос поиска товаров
    if (!s.compare(L"s") || !s.compare(L"search"))
    {
        std::wcout << L"Введите наименование товара или его штрих-код" << L"\n";
        std::wstring task;
        // Запрос поисковых данных
        std::getline(std::wcin, task);
        std::vector<int> indexv = this->findAllIndexesByNameOrBarcode(task);

        // Если не было найдено ни одного товара
        if (!indexv.size())
        {
            std::wcout << L"Товаров с таким названием или штих-кодом не найдено!" << L"\n";
            std::wcout << L"Поиск закрыт. Если не знаете что вводить, введите help." << L"\n";
            return true;
        }
        std::wcout << L"Найдено " << indexv.size() << L" товаров:" << L"\n";
        int page = 0;
        this->loadProductBaseFromSearch(page, indexv);
        std::wcout << L"Введите номер из списка товаров. Если не знаете что вводить, введите help." << L"\n";
        while (true)
        {
            // Запрос номера товара из списка товаров или другого запроса
            std::getline(std::wcin, task);

            // Если запрос помощи, вывод вспомогательной информации
            if (!task.compare(L"h") || !task.compare(L"help"))
            {
                this->searchHelpTask();
                continue;
            }

            // Если запрос отмены, закрываем поиск
            if (!task.compare(L"c") || !task.compare(L"cancel"))
            {
                std::wcout << L"Поиск закрыт. Если не знаете что вводить, введите help." << L"\n";
                break;
            }
            int psz = indexv.size();
            int sz = psz % 5 ? psz - psz % 5 + 5 : psz;

            // Если запрос списка, вновь выводим список товаров
            if (!task.compare(L"l") || !task.compare(L"list"))
            {
                this->loadProductBaseFromSearch(page % (sz / 5), indexv);
                continue;
            }

            // Если запрос следующей страницы, выводим следующую страницу товаров
            if (!task.compare(L"n") || !task.compare(L"next"))
            {
                page = page == sz / 5 - 1 ? 0 : page + 1;
                this->loadProductBaseFromSearch(page % (sz / 5), indexv);
                continue;
            }

            // Если запрос предыдущей страницы, выводим предыдущую страницу товаров
            if (!task.compare(L"p") || !task.compare(L"prev"))
            {
                page = page ? page - 1 : sz / 5 - 1;
                this->loadProductBaseFromSearch(page % (sz / 5), indexv);
                continue;
            }
            int index;
            try
            {
                // Получаем номер товара
                index = std::stoi(task);
            }
            catch(const std::exception& e)
            {
                std::wcout << L"Номер введён неверно. Если не знаете что вводить, введите help." << L"\n";
                continue;
            }

            // Если номер в пределах товарной базы
            if (index > 0 && index <= this->products.size())
            {
                // Если товар закончился
                if (this->getProductAmountByIndex(index - 1) < 1)
                {
                    std::wcout << L"Товар закончился, невозможно добавить в чек" << L"\n";
                    continue;
                }
                
                this->addProductToCheck(index);
                std::wcout << L"Поиск завершён. Если не знаете что вводить, введите help." << L"\n";
                break;
            }
            std::wcout << L"Номер введён неверно. Если не знаете что вводить, введите help." << L"\n";
        }
        return true;
    }

    // Если запрос закрытия чека
    if (!s.compare(L"cc") || !s.compare(L"close check"))
    {
        // Если в чеке отсутствуют товарные позиции
        if (!this->productIndex.size())
        {
            std::wcout << L"Вы ничего не заказали! Чтобы закрыть чек, добавьте в него хотябы одну позицию." << L"\n";
            return true;
        }
        std::wcout << L"Каким способом будет производиться оплата: наличными (1/cash) или картой (2/card)?" << L"\n";
        std::wstring task;
        float total_price = 0.0f;
        for (int i = 0; i < this->productIndex.size(); i++)
        {
            total_price += this->products[this->productIndex[i]]->getPrice() * this->productAmount[i];
        }
        while (true)
        {
            // Запрос типа оплаты
            std::getline(std::wcin, task);

            // Если оплата наличными
            if (!task.compare(L"1") || !task.compare(L"cash"))
            {
                std::wstring buy_task;
                while(true)
                {
                    std::wcout << L"Цена покупки: " << total_price << L" руб. Введите сумму оплаты или отмените закрытие чека [n]:" << L"\n";
                    // Запрос суммы оплаты или отмены покупки
                    std::getline(std::wcin, buy_task);

                    // Если отмена покупки
                    if (!buy_task.compare(L"n") || !buy_task.compare(L"N"))
                    {
                        std::wcout << L"Покупка отменена. Если не знаете что вводить, введите help." << L"\n";
                        return true;
                    }
                    float payment;
                    try
                    {
                        // Получение суммы оплаты
                        payment = std::stof(buy_task);
                    }
                    catch(const std::exception& e)
                    {
                        std::wcout << L"Сумма введена неверно." << L"\n";
                        continue;
                    }

                    // Если недостаточно средств
                    if (std::isgreater(total_price, payment))
                    {
                        std::wcout << L"Недостаточно средств!" << L"\n";
                        continue;
                    }

                    // Если в кассе недостаточно средств
                    if (std::isgreater(payment - total_price, this->cashReg->getCash()))
                    {
                        std::wcout << L"Операция невозможна, в кассе недостаточно денег ("
                                    << std::setprecision(2) << this->cashReg->getCash() << L") для сдачи." << L"\n";
                        continue;
                    }

                    // Работа с кассой
                    this->cashReg->increaseCash(payment);
                    this->cashReg->decreaseCash(payment - total_price);
                    // Работа со статистикой
                    this->stat->newCashCheck(total_price);

                    std::wcout << L"Спасибо за покупку! Печатаем чек:" << L"\n";

                    // Печать чека
                    this->printCheck(total_price, payment, false);
                    // Обновление базы товаров
                    this->updateProductFile();

                    std::wcout << L"Если не знаете что вводить, введите help." << L"\n";
                    this->next_task = CASHIER;
                    return true;
                }
                break;
            }

            // Если оплата картой
            if (!task.compare(L"2") || !task.compare(L"card"))
            {
                std::wstring buy_task;
                while(true)
                {
                    std::wcout << L"Цена покупки: L" << total_price << L" руб. Совершаем покупку [y/n]?" << L"\n";
                    // Запрос совершения покупки
                    std::getline(std::wcin, buy_task);

                    // Если ответ положительный
                    if (!buy_task.compare(L"y") || !buy_task.compare(L"Y"))
                    {
                        // Работа со статистикой
                        this->stat->newCardCheck(total_price);

                        std::wcout << L"Спасибо за покупку! Печатаем чек:" << L"\n";

                        // Печать чека
                        this->printCheck(total_price, total_price, true);
                        // Обновление базы товаров
                        this->updateProductFile();

                        std::wcout << L"Если не знаете что вводить, введите help." << L"\n";
                        this->next_task = CASHIER;
                        return true;
                    }
                    // Если ответ отрицательный
                    if (!buy_task.compare(L"n") || !buy_task.compare(L"N"))
                    {
                        std::wcout << L"Покупка отменена. Если не знаете что вводить, введите help." << L"\n";
                        return true;
                    }
                }
                break;
            }
            std::wcout << L"Введите, каким способом будет производиться оплата: наличными (1/cash) или картой (2/card)?" << L"\n";
        }
    }

    return false;
};

// Уменьшение регистра символа
void charToLower(wchar_t& c)
{
    c = std::tolower(c);
};

// Уменьшение регистра строки
void strToLower(std::wstring& s)
{
    std::for_each(s.begin(), s.end(), charToLower);
};

// Запрос выхода
bool isExit(const std::wstring& s)
{
    return !s.compare(L"e") || !s.compare(L"exit");
};

// Чтение строки товарной базы
std::vector<std::wstring> readLine(const std::wstring &line)
{
    std::vector<std::wstring> result;
    std::wstring cell;
    std::wstringstream ls(line);

    while (std::getline(ls, cell, L','))
    {
        result.push_back(cell);
    }
    
    std::size_t pos;
    // Если в базе лишние параметры или параметров не хватает
    if (result.size() != 4)
    {
        throw std::wstring{L"Неверное количество параметров в строке "};
    }
    int amount = 0;
    try
    {
        // получение количества товаров
        amount = std::stoi(result[1], &pos);
    }
    catch(const std::exception &)
    {
        throw std::wstring{L"Невозможно прочитать количество товаров в строке "};
    }
    // Если не число
    if (pos != result[1].size())
    {
        throw std::wstring{L"Невозможно прочитать количество товаров в строке "};
    }
    // Если число товаров меньше нуля
    if (amount < 0)
    {
        throw std::wstring{L"Слишком мало товара в строке "};
    }

    float price;
    try
    {
        // Получение цены
        price = std::stof(result[3], &pos);
    }
    catch(const std::exception& e)
    {
        throw std::wstring{L"Невозможно прочитать цену товаров в строке "};
    }
    // Если цена не число
    if (pos != result[3].size())
    {
        throw std::wstring{L"Невозможно прочитать цену товаров в строке "};
    }
    // Если цена меньше 0
    if (std::isgreater(0.01f, price))
    {
        throw std::wstring{L"Цена слишком мала в строке "};
    }

    return result;
}

// Основной поток
int main()
{
    setlocale(LC_ALL, "");
    // Чтение файла
    std::wfstream file;
    if (!std::filesystem::exists(L"prodcd.csv"))
    {
        std::wcerr << L"Файла базы товаров с именем prodcd.csv не существует!";
        std::wcin.get();
        return 0;
    }
    file.open(L"prodcd.csv", std::ios::in);
    std::vector<Product*> productv;
    std::vector<std::vector<std::wstring>> allLinesv;
    int i = 0;
    std::wstring temp;
    while (std::getline(file, temp))
    {
        std::vector<std::wstring> linev;
        try
        {
            linev = readLine(temp);
        }
        catch(const std::wstring& s)
        {
            std::wcerr << s << i + 1;
            std::wcin.get();
            file.close();
            return 0;
        }
        allLinesv.push_back(linev);
        i++;
    }
    file.close();

    for (const auto &line : allLinesv)
    {
        productv.push_back(new Product(line[0], std::stoi(line[1]), line[2], std::stof(line[3])));
    }

    // Создание объектов
    CashReg* cashReg = new CashReg();
    std::map<task_type, Task*> taskmap;
    taskmap[MAIN] = new MainTask(cashReg);
    taskmap[OS] = new OSTask(cashReg);
    auto checkStat = new CheckStat();
    auto cashier = new CashierTask(cashReg);
    auto check = new CheckTask(cashReg, productv);
    cashier->setCheckStat(checkStat);
    check->setCheckStat(checkStat);
    taskmap[CASHIER] = cashier;
    taskmap[NEW_CHECK] = check;
    Task* currentTask = taskmap[MAIN];

    std::wstring task = L"";
    std::wcout << L"Рабочее место кассира. Если не знаете что вводить, введите help." << L"\n";

    // Основной цикл запросов
    while (true)
    {
        std::getline(std::wcin, task);
        strToLower(task);
        if (!currentTask->completeTask(task))
        {
            if (currentTask->isExit()) break;
            std::wcout << L"Введена неизвестная команда, проверьте правильность написания или воспользуйтесь командой help." << L"\n";
            continue;
        }
        auto next_type = currentTask->getNextTaskType();
        if (currentTask->getCurrentTaskType() != taskmap[next_type]->getCurrentTaskType())
        {
            currentTask->setNextTaskType(currentTask->getCurrentTaskType());
            currentTask = taskmap[next_type];
        }
    }

    // Удаление объектов
    for (auto const& [key, val] : taskmap)
    {
        delete val;
    }
    for (auto const& p : productv)
    {
        delete p;
    }
    delete cashReg;
    delete checkStat;
    // Конец программы
    std::wcout << L"Конец выполнения программы";
}



