#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <math.h>

using namespace std;

#define TRUE 1
#define FALSE 0

int marks[4], standard, roll_no, totalMarks = 0, averageMarks = 0, percentage = 0, max, min, count = 1;
char name[50], ch;

class Logger
{
public:
    static void monitor_filesize();
    static void log(string, string);

private:
    static mutex myfilemutex;
    static bool filesize_monitoring;
    static void filesize();
    static void logger(string, string);
};

void Logger::logger(string message, string name)
{
    time_t now = time(0);
    struct tm *currentTime = localtime(&now);

    char datetime[20];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", currentTime);

    ofstream logfile;
    logfile.open("log.txt", ios::app);
    logfile << "**************************************************************************" << endl;

    if (message == "add")
    {
        logfile << "Added details of " << name << " to the table " << endl
                << "Date and time: " << datetime << endl;
    }
    else if (message == "get")
    {
        logfile << "Displayed details of " << name << " from the table " << endl
                << "Date and time: " << datetime << endl;
    }
    else if (message == "delete")
    {
        logfile << "Deleted details of " << name << " from the table " << endl
                << "Date and time: " << datetime << endl;
    }
    else if (message == "modify")
    {
        logfile << "Modified the details of " << name << " in the table " << endl
                << "Date and time: " << datetime << endl;
    }
    else if (message == "max")
    {
        logfile << "Displayed details of " << name << " who scored maximum marks from the table " << endl
                << "Date and time: " << datetime << endl;
    }
    else if (message == "min")
    {
        logfile << "Displayed details of " << name << " who scored minimum marks from the table " << endl
                << "Date and time: " << datetime << endl;
    }
    else if (message == "search")
    {
        logfile << "Searched the details of " << name << " from the table " << endl
                << "Date and time: " << datetime << endl;
    }
    logfile << "**************************************************************************" << endl
            << endl;

    logfile.close();
}

mutex Logger::myfilemutex;
bool Logger::filesize_monitoring = false;

void Logger::filesize()
{
    ifstream file("log.txt", ios::binary | ios::ate);
    if (file)
    {
        lock_guard<mutex> lock(myfilemutex);
        streampos fileSize = file.tellg();
        file.close();

        if (fileSize > 1024)
        {
            cout << "Filesize is greater than 1mb" << endl;
            remove("log.txt");
            file.open("log.txt", ios::app);
        }
        else
        {
            cout << "Filesize is less than 1mb" << endl;
        }
    }
    else
    {
        cout << "Failed to open the file." << endl;
    }
}

void Logger::log(string message, string name)
{
    thread myloggerthread(logger, message, name);
    myloggerthread.detach();
}

void Logger::monitor_filesize()
{
    if (!filesize_monitoring)
    {
        thread myfilesize(filesize);
        myfilesize.detach();
    }
}

class Student;

class StudentFactory
{
public:
    static Student *CreateStudent(int standard, const std::string &name);
    int studentstandard_validation(int);
    int studentroll_no_validation(int);
    int studentname_validation(char *);
    int studentmarks_validation(int);
};

class Student
{

public:
    Student(const std::string &studentName) : name(studentName)
    {
    }
    virtual ~Student()
    {
    }
    string name;
    int max, min, i;

    void AddSubject(const std::string &subject, int *marks)
    {
        string message = "add";
        Logger::log(message, name);
        pqxx::connection conn("dbname=student_record user=postgres password=1234");
        if (!conn.is_open())
        {
            std::cout << "Failed to connect to PostgreSQL" << std::endl;
        }
        string query1 = "INSERT INTO standard1 ( roll_no, standard, name, english, maths, hindi, science, TotalMarks, AverageMarks, Percentage) VALUES ('" + to_string(roll_no) + "', '" + to_string(standard) + "' , '" + name + "' , '" + to_string(marks[0]) + "' , '" + to_string(marks[1]) + "' , '" + to_string(marks[2]) + "' , '" + to_string(marks[3]) + "' , '" + to_string(totalMarks) + "' , '" + to_string(averageMarks) + "' ,'" + to_string(percentage) + "') ";
        cout << "DATABASE ADDED SUCCESSFULLY" << endl;
        pqxx::work txn(conn);
        txn.exec(query1);
        txn.commit();
    }

    void GetDetails()
    {
        int standard, roll_no;
        string message = "get";
        cout << "****************GET STUDENT DETAILS****************" << endl;
        cout << "Enter the standard: ";
        cin >> standard;
        cout << "Enter the roll_no: ";
        cin >> roll_no;
        cout << endl;
        Logger::log(message, name);
        pqxx::connection conn("dbname=student_record user=postgres password=1234");
        if (!conn.is_open())
        {
            std::cout << "Failed to connect to PostgreSQL" << std::endl;
        }
        string query2 = "SELECT * FROM standard1 WHERE standard = '" + to_string(standard) + "' AND roll_no = " + to_string(roll_no);
        pqxx::nontransaction nonTxn(conn);
        pqxx::result result(nonTxn.exec(query2));

        for (const auto &row : result)
        {
            std::cout << "***********************************************" << endl;
            std::cout << "Standard: " << row["standard"].as<int>() << std::endl;
            std::cout << "Rollno: " << row["roll_no"].as<int>() << std::endl;
            std::cout << "Name: " << row["name"].as<std::string>() << std::endl;
            std::cout << "English: " << row["english"].as<int>() << std::endl;
            std::cout << "Maths: " << row["maths"].as<int>() << std::endl;
            std::cout << "Hindi: " << row["hindi"].as<int>() << std::endl;
            std::cout << "Science: " << row["science"].as<int>() << std::endl;
            std::cout << "TotalMarks: " << row["totalmarks"].as<int>() << std::endl;
            std::cout << "AverageMarks: " << row["averagemarks"].as<int>() << std::endl;
            std::cout << "Percentage: " << row["percentage"].as<int>() << std::endl;
            std::cout << "************************************************" << endl;
        }
    }

    void DeleteStudent()
    {
        string message = "delete";
        Logger::log(message, name);
        cout << "****************DELETE STUDENT DETAILS****************" << endl;
        cout << "Enter rollno: ";
        cin >> roll_no;
        cout << "Enter standard: ";
        cin >> standard;
        cout << endl;
        pqxx::connection conn("dbname=student_record user=postgres password=1234");
        if (!conn.is_open())
        {
            std::cout << "Failed to connect to PostgreSQL" << std::endl;
        }
        std::string query3 = "DELETE FROM standard1 WHERE standard = " + to_string(standard) + " AND roll_no = " + to_string(roll_no);
        cout << "DATABASE DELETED SUCCESSFULLY" << endl;
        pqxx::work txn(conn);
        txn.exec(query3);
        txn.commit();
    }

    void modify_details()
    {
        StudentFactory Sf;
        string message = "modify";
        cout << "****************MODIFY STUDENT DETAILS****************" << endl;
        cout << "Enter the standard: ";
        cin >> standard;
        cout << "Enter the roll_no: ";
        cin >> roll_no;
        Logger::log(message, name);
        pqxx::connection conn("dbname=student_record user=postgres password=1234");
        if (!conn.is_open())
        {
            std::cout << "Failed to connect to PostgreSQL" << std::endl;
        }

        while (1)
        {
            cout << "Enter updated marks for the following subjects" << endl;
            cout << "Enter English Marks: ";
            cin >> marks[0];
            if (Sf.studentmarks_validation(marks[0]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Maths Marks: ";
            cin >> marks[1];
            if (Sf.studentmarks_validation(marks[1]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Hindi Marks: ";
            cin >> marks[2];
            if (Sf.studentmarks_validation(marks[2]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Science Marks: ";
            cin >> marks[3];
            if (Sf.studentmarks_validation(marks[3]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }

        for (int i = 0; i < 4; i++)
        {
            totalMarks += marks[i];
        }

        max = marks[0];
        min = marks[0];
        for (i = 1; i < 4; i++)
        {
            // If current element is greater than max
            if (marks[i] > max)
                max = marks[i];

            // If current element is smaller than min
            if (marks[i] < min)
                min = marks[i];
        }

        averageMarks = totalMarks / 4;
        percentage = (totalMarks * 100) / 400;

        string update_query = "UPDATE standard1 SET english = '" + to_string(marks[0]) + "', maths = '" + to_string(marks[1]) + "', hindi = '" + to_string(marks[2]) + "', science = '" + to_string(marks[3]) + "', totalMarks = '" + to_string(totalMarks) + "', averageMarks = '" + to_string(averageMarks) + "', percentage = '" + to_string(percentage) + "' WHERE roll_no = '" + to_string(roll_no) + "' AND standard = '" + to_string(standard) + "'";

        pqxx::work txn(conn);
        txn.exec(update_query);
        txn.commit();
        Logger::log(message, name);
        totalMarks = 0;

        cout << endl << "DATABASE MODIFIED SUCCESSFULLY" << endl;
    }

    void max_marks()
    {
        string message = "max";
        int num, count = 1;
        cout << "****************HIGHEST MARKS****************" << endl;
        cout << "Enter number of outputs required: ";
        cin >> num;
        cout << "Enter standard: ";
        cin >> standard;
        pqxx::connection conn("dbname=student_record user=postgres password=1234 hostaddr=127.0.0.1 port=5432");
        pqxx::work txn(conn);
        pqxx::result res = txn.exec("SELECT * FROM standard1 WHERE standard = " + to_string(standard) + "ORDER BY totalmarks DESC LIMIT " + to_string(num));
        for (const auto &row : res)
        {
            cout << endl;
            cout << "*****************************************" << endl;
            cout << "Count: " << count << endl;
            std::cout << "standard: " << row["standard"].as<int>() << std::endl;
            std::cout << "Rollno: " << row["roll_no"].as<int>() << std::endl;
            std::cout << "Name: " << row["name"].as<std::string>() << std::endl;
            std::cout << "English: " << row["english"].as<int>() << std::endl;
            std::cout << "Maths: " << row["maths"].as<int>() << std::endl;
            std::cout << "Hindi: " << row["hindi"].as<int>() << std::endl;
            std::cout << "Science: " << row["science"].as<int>() << std::endl;
            std::cout << "TotalMarks: " << row["totalmarks"].as<int>() << std::endl;
            std::cout << "AverageMarks: " << row["averagemarks"].as<int>() << std::endl;
            std::cout << "percentage: " << row["percentage"].as<int>() << std::endl;
            cout << "*****************************************" << endl;
            count++;
            Logger::log(message, name);
        }
        txn.commit();
    }

    void min_marks()
    {
        string message = "min";
        int num, count = 1;
        cout << "****************LOWEST MARKS****************" << endl;
        cout << "Enter number of outputs required: ";
        cin >> num;
        cout << "Enter Standard: ";
        cin >> standard;
        pqxx::connection conn("dbname=student_record user=postgres password=1234 hostaddr=127.0.0.1 port=5432");
        pqxx::work txn(conn);
        pqxx::result res = txn.exec("SELECT * FROM standard1 WHERE standard = " + to_string(standard) + "ORDER BY totalmarks ASC LIMIT " + to_string(num));
        for (const auto &row : res)
        {
            std::cout << endl;
            std::cout << "*****************************************" << endl;
            std::cout << "Count: " << count << endl;
            std::cout << "standard: " << row["standard"].as<int>() << std::endl;
            std::cout << "Rollno: " << row["roll_no"].as<int>() << std::endl;
            std::cout << "Name: " << row["name"].as<std::string>() << std::endl;
            std::cout << "English: " << row["english"].as<int>() << std::endl;
            std::cout << "Maths: " << row["maths"].as<int>() << std::endl;
            std::cout << "Hindi: " << row["hindi"].as<int>() << std::endl;
            std::cout << "Science: " << row["science"].as<int>() << std::endl;
            std::cout << "TotalMarks: " << row["totalmarks"].as<int>() << std::endl;
            std::cout << "AverageMarks: " << row["averagemarks"].as<int>() << std::endl;
            std::cout << "percentage: " << row["percentage"].as<int>() << std::endl;
            std::cout << "*****************************************" << endl;
            count++;
            Logger::log(message, name);
        }
        txn.commit();
    }

    void search_by_character()
    {
        string message = "search";
        cout << "****************SEARCH BY CHARACTER****************" << endl;
        cout << "Enter  the character: ";
        cin >> ch;
        cout << "Enter Standard: ";
        cin >> standard;
        pqxx::connection conn("dbname=student_record user=postgres password=1234 port=5432");
        pqxx::work txn(conn);
        pqxx::result res = txn.exec("SELECT * FROM standard1 WHERE standard = " + to_string(standard) + " AND name ILIKE '" + string(1, ch) + "%'");
        if (!res.empty())
        {
            for (const auto &row : res)
            {
                std::cout << endl;
                std::cout << "*****************************************" << endl;
                std::cout << "Count: " << count << endl;
                std::cout << "standard: " << row["standard"].as<int>() << std::endl;
                std::cout << "Rollno: " << row["roll_no"].as<int>() << std::endl;
                std::cout << "Name: " << row["name"].as<std::string>() << std::endl;
                std::cout << "English: " << row["english"].as<int>() << std::endl;
                std::cout << "Maths: " << row["maths"].as<int>() << std::endl;
                std::cout << "Hindi: " << row["hindi"].as<int>() << std::endl;
                std::cout << "Science: " << row["science"].as<int>() << std::endl;
                std::cout << "TotalMarks: " << row["totalmarks"].as<int>() << std::endl;
                std::cout << "AverageMarks: " << row["averagemarks"].as<int>() << std::endl;
                std::cout << "percentage: " << row["percentage"].as<int>() << std::endl;
                std::cout << "*****************************************" << endl;
                count++;
                Logger::log(message, name);
            }
        }
        else
        {
            cout << "No name found starting with '" << ch << "' in " << standard << "standard1" << endl;
            cout << endl;
        }

        txn.commit();
    }

    virtual void AddSubjectSpecific() = 0;
};

class StudentStandard1 : public Student
{
public:
    StudentFactory Sf;
    StudentStandard1(const std::string &name) : Student(name) {}
    void AddSubjectSpecific() override
    {
        std::string subject;
        while (1)
        {
            cout << "Enter English Marks: ";
            cin >> marks[0];
            if (Sf.studentmarks_validation(marks[0]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Maths Marks: ";
            cin >> marks[1];
            if (Sf.studentmarks_validation(marks[1]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Hindi Marks: ";
            cin >> marks[2];
            if (Sf.studentmarks_validation(marks[2]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Science Marks: ";
            cin >> marks[3];
            if (Sf.studentmarks_validation(marks[3]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }

        for (int i = 0; i < 4; i++)
        {
            totalMarks += marks[i];
        }
        max = marks[0];
        min = marks[0];
        for (i = 1; i < 4; i++)
        {
            // If current element is greater than max
            if (marks[i] > max)
                max = marks[i];

            // If current element is smaller than min
            if (marks[i] < min)
                min = marks[i];
        }

        averageMarks = totalMarks / 4;
        percentage = (totalMarks * 100) / 400;

        cout << endl << "Total Marks: " << totalMarks << endl;
        cout << "Average Marks: " << averageMarks << endl;
        cout << "Percentage: " << percentage << endl;
        cout << "Maximum marks =" << max << endl;
        cout << "Minimum marks =" << min << endl << endl;
        AddSubject(subject, marks);
    }
};

class StudentStandard2 : public Student
{
public:
    StudentFactory Sf;
    string name;
    StudentStandard2(const std::string &name) : Student(name) {}
    void AddSubjectSpecific() override
    {
        std::string name;
        std::string subject;
        while (1)
        {
            cout << "Enter English Marks: ";
            cin >> marks[0];
            if (Sf.studentmarks_validation(marks[0]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Maths Marks: ";
            cin >> marks[1];
            if (Sf.studentmarks_validation(marks[1]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Hindi Marks: ";
            cin >> marks[2];
            if (Sf.studentmarks_validation(marks[2]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Science Marks: ";
            cin >> marks[3];
            if (Sf.studentmarks_validation(marks[3]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }

        for (int i = 0; i < 4; i++)
        {
            totalMarks += marks[i];
        }
        max = marks[0];
        min = marks[0];
        for (i = 1; i < 4; i++)
        {
            // If current element is greater than max
            if (marks[i] > max)
                max = marks[i];

            // If current element is smaller than min
            if (marks[i] < min)
                min = marks[i];
        }

        averageMarks = totalMarks / 4;
        percentage = (totalMarks * 100) / 400;

        cout << endl << "Total Marks: " << totalMarks << endl;
        cout << "Average Marks: " << averageMarks << endl;
        cout << "Percentage: " << percentage << endl;
        cout << "Maximum marks =" << max << endl;
        cout << "Minimum marks =" << min << endl << endl;
        AddSubject(subject, marks);
    }
};

class StudentStandard3 : public Student
{
public:
    StudentFactory Sf;
    StudentStandard3(const std::string &name) : Student(name)
    {
    }
    void AddSubjectSpecific() override
    {
        std::string subject;
        while (1)
        {
            cout << "Enter English Marks: ";
            cin >> marks[0];
            if (Sf.studentmarks_validation(marks[0]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Maths Marks: ";
            cin >> marks[1];
            if (Sf.studentmarks_validation(marks[1]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Hindi Marks: ";
            cin >> marks[2];
            if (Sf.studentmarks_validation(marks[2]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Science Marks: ";
            cin >> marks[3];
            if (Sf.studentmarks_validation(marks[3]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        // Displaying the entered mark

        for (int i = 0; i < 4; i++)
        {
            totalMarks += marks[i];
        }
        max = marks[0];
        min = marks[0];
        for (i = 1; i < 4; i++)
        {
            // If current element is greater than max
            if (marks[i] > max)
                max = marks[i];

            // If current element is smaller than min
            if (marks[i] < min)
                min = marks[i];
        }

        averageMarks = totalMarks / 4;
        percentage = (totalMarks * 100) / 400;

        cout << endl << "Total Marks: " << totalMarks << endl;
        cout << "Average Marks: " << averageMarks << endl;
        cout << "Percentage: " << percentage << endl;
        cout << "Maximum marks =" << max << endl;
        cout << "Minimum marks =" << min << endl << endl;
        AddSubject(subject, marks);
    }
};

class StudentStandard4 : public Student
{
public:
    StudentFactory Sf;
    StudentStandard4(const std::string &name) : Student(name) {}
    void AddSubjectSpecific() override
    {
        std::string subject;
        while (1)
        {
            cout << "Enter English Marks: ";
            cin >> marks[0];
            if (Sf.studentmarks_validation(marks[0]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Maths Marks: ";
            cin >> marks[1];
            if (Sf.studentmarks_validation(marks[1]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Hindi Marks: ";
            cin >> marks[2];
            if (Sf.studentmarks_validation(marks[2]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        while (1)
        {
            cout << "Enter Science Marks: ";
            cin >> marks[3];
            if (Sf.studentmarks_validation(marks[3]) == 1)
                break;
            else
                cout << "Invalid marks" << endl;
        }
        // Displaying the entered mark

        for (int i = 0; i < 4; i++)
        {
            totalMarks += marks[i];
        }
        max = marks[0];
        min = marks[0];
        for (i = 1; i < 4; i++)
        {
            // If current element is greater than max
            if (marks[i] > max)
                max = marks[i];

            // If current element is smaller than min
            if (marks[i] < min)
                min = marks[i];
        }

        averageMarks = totalMarks / 4;
        percentage = (totalMarks * 100) / 400;

        cout << endl << "Total Marks: " << totalMarks << endl;
        cout << "Average Marks: " << averageMarks << endl;
        cout << "Percentage: " << percentage << endl;
        cout << "Maximum marks =" << max << endl;
        cout << "Minimum marks =" << min << endl << endl;
        AddSubject(subject, marks);
    }
};

Student *StudentFactory::CreateStudent(int standard, const std::string &name)
{
    if (standard == 1)
    {
        return new StudentStandard1(name);
        return nullptr;
    }
    else if (standard == 2)
    {
        return new StudentStandard2(name);
        return nullptr;
    }
    else if (standard == 3)
    {
        return new StudentStandard3(name);
        return nullptr;
    }
    else if (standard == 4)
    {
        return new StudentStandard4(name);
        return nullptr;
    }
    else
        return nullptr;
}

int StudentFactory::studentstandard_validation(int standard)
{
    if (standard >= 1 && standard <= 4)
        return TRUE;
    else
        return FALSE;
}

int StudentFactory::studentroll_no_validation(int roll_no)
{
    if (roll_no >= 1 && roll_no <= 40)
        return TRUE;
    else
        return FALSE;
}

int StudentFactory::studentname_validation(char *name)
{
    int len, i;
    int small = 0, caps = 0, num = 0, spec_char = 0;
    len = strlen(name);
    if (len <= 0)
    {
        return FALSE;
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            if (name[i] >= 'a' && name[i] <= 'z')
                small++;
            else if (name[i] >= 'A' && name[i] <= 'Z')
                caps++;
            else if (name[i] >= '0' && name[i] <= '9')
                num++;
            else if (name[i] >= '!' && name[i] <= '*')
                spec_char++;
        }
    }
    if ((num >= 1) || (spec_char >= 1) || (small <= 0) || (caps <= 0))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

int StudentFactory::studentmarks_validation(int marks)
{
    if (marks >= 1 && marks <= 100)
        return TRUE;
    else
        return FALSE;
}

int main()
{
    StudentFactory Sf;
    Logger::monitor_filesize();

    pqxx::connection conn("dbname=student_record user=postgres password=1234");
    if (conn.is_open())
    {
        std::cout << "Connected to PostgreSQL!" << std::endl;
    }
    else
    {
        std::cout << "Failed to connect to PostgreSQL" << std::endl;
    }
    cout << "***************************STUDENT RECORD APPLICATION*******************************" << endl;
    while (1)
    {
        cout << "Enter Standard: ";
        cin >> standard;
        if (Sf.studentstandard_validation(standard) == 1)
            break;
        else
            cout << "Invalid Standard" << endl;
    }

    Student *student = StudentFactory::CreateStudent(standard, name);
    if (student == nullptr)
    {
        return 0;
    }

    while (1)
    {
        int choice;
        cout << endl << "****************STUDENT MENU****************" << endl;
        cout << "1. Add details\n2. Get details\n3. Delete student\n4. Highest Marks\n5. Lowest Marks\n6. Search_by_character\n7. Modify student\n0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        system("clear");
        if (choice == 1)
        {
            while (1)
            {
                cout << "****************ADD STUDENT DETAILS****************" << endl;
                cout << "Enter Standard: ";
                cin >> standard;
                if (Sf.studentstandard_validation(standard) == 1)
                    break;
                else
                    cout << "Invalid Standard" << endl;
            }
            while (1)
            {
                cout << "Enter roll_no: ";
                cin >> roll_no;
                pqxx::work txn(conn);
                pqxx::result res = txn.exec("SELECT * FROM  standard1 WHERE roll_no = " + to_string(roll_no) + "AND standard = " + to_string(standard));
                if (!(res.empty()))
                {
                    cout << "Roll no exists" << endl;
                }
                else
                {
                    break;
                }
            }
            while (1)
            {
                cout << "Enter the student name: ";
                cin >> name;
                if (Sf.studentname_validation(name) == 1)
                {
                    student->name = name;
                    break;
                }
                else
                {
                    cout << "Invalid name" << endl;
                }
            }
            student->AddSubjectSpecific();
        }
        else if (choice == 2)
        {
            student->GetDetails();
        }
        else if (choice == 3)
        {
            student->DeleteStudent();
        }
        else if (choice == 4)
        {
            student->max_marks();
        }
        else if (choice == 5)
        {
            student->min_marks();
        }
        else if (choice == 6)
        {
            student->search_by_character();
        }
        else if (choice == 7)
        {
            student->modify_details();
        }
        else if (choice == 0)
        {
            exit(0);
        }
        else
        {
            cout << "Invalid choice. Please try again." << std::endl;
        }
    }
    while (true)
    {
        if (standard == 1)
        {
            int choice;
            cout << endl << "****************STUDENT MENU****************" << endl;
            cout << "1. Add details\n2. Get details\n3. Delete student\n4. Highest Marks\n5. Lowest Marks\n6. Search_by_character\n7. Modify student\n0. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;
            if (choice == 1)
            {
                while (1)
                {
                    cout << "Enter Standard: ";
                    cin >> standard;
                    if (Sf.studentstandard_validation(standard) == 1)
                        break;
                    else
                        cout << "Invalid Standard" << endl;
                }
                while (1)
                {
                    cout << "Enter roll_no: ";
                    cin >> roll_no;
                    pqxx::work txn(conn);
                    pqxx::result res = txn.exec("SELECT * FROM  standard1 WHERE roll_no = " + to_string(roll_no) + "AND standard = " + to_string(standard));
                    if (!(res.empty()))
                    {
                        cout << "Roll no exists" << endl;
                    }
                    else
                    {
                        break;
                    }
                }
                while (1)
                {
                    cout << "Enter the student name: ";
                    cin >> name;
                    if (Sf.studentname_validation(name) == 1)
                    {
                        student->name = name;
                        break;
                    }
                    else
                    {
                        cout << "Invalid name" << endl;
                    }
                }
                student->AddSubjectSpecific();
            }
            else if (choice == 2)
            {
                student->GetDetails();
            }
            else if (choice == 3)
            {
                student->DeleteStudent();
            }
            else if (choice == 4)
            {
                student->max_marks();
            }
            else if (choice == 5)
            {
                student->min_marks();
            }
            else if (choice == 6)
            {
                student->search_by_character();
            }
            else if (choice == 7)
            {
                student->modify_details();
            }
            else if (choice == 0)
            {
                exit(0);
            }
            else
            {
                cout << "Invalid choice. Please try again." << std::endl;
            }
        }
        else if (standard == 2)
        {
            int choice;
            cout << endl << "****************STUDENT MENU****************" << endl;
            cout << "1. Add details\n2. Get details\n3. Delete student\n4. Highest Marks\n5. Lowest Marks\n6. Search_by_character\n7. Modify student\n0. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;
            if (choice == 1)
            {
                while (1)
                {
                    cout << "Enter Standard: ";
                    cin >> standard;
                    if (Sf.studentstandard_validation(standard) == 1)
                        break;
                    else
                        cout << "Invalid Standard" << endl;
                }
                while (1)
                {
                    cout << "Enter roll_no: ";
                    cin >> roll_no;
                    pqxx::work txn(conn);
                    pqxx::result res = txn.exec("SELECT * FROM  standard1 WHERE roll_no = " + to_string(roll_no) + "AND  standard = " + to_string(standard));
                    if (!(res.empty()))
                    {
                        cout << "Roll no exists" << endl;
                    }
                    else
                    {
                        break;
                    }
                }
                while (1)
                {
                    cout << "Enter the student name: ";
                    cin >> name;
                    if (Sf.studentname_validation(name) == 1)
                    {
                        student->name = name;
                        break;
                    }
                    else
                    {
                        cout << "Invalid name" << endl;
                    }
                }
                student->AddSubjectSpecific();
            }
            else if (choice == 2)
            {
                student->GetDetails();
            }
            else if (choice == 3)
            {
                student->DeleteStudent();
            }
            else if (choice == 4)
            {
                student->max_marks();
            }
            else if (choice == 5)
            {
                student->min_marks();
            }
            else if (choice == 6)
            {
                student->search_by_character();
            }
            else if (choice == 7)
            {
                student->modify_details();
            }
            else if (choice == 0)
            {
                exit(0);
            }
            else
            {
                cout << "Invalid choice. Please try again." << std::endl;
            }
        }
        else if (standard == 3)
        {
            int choice;
            cout << endl << "****************STUDENT MENU****************" << endl;
            cout << "1. Add details\n2. Get details\n3. Delete student\n4. Highest Marks\n5. Lowest Marks\n6. Search_by_character\n7. Modify student\n0. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;
            if (choice == 1)
            {
                while (1)
                {
                    cout << "Enter Standard: ";
                    cin >> standard;
                    if (Sf.studentstandard_validation(standard) == 1)
                        break;
                    else
                        cout << "Invalid Standard" << endl;
                }
                while (1)
                {
                    cout << "Enter roll_no: ";
                    cin >> roll_no;
                    pqxx::work txn(conn);
                    pqxx::result res = txn.exec("SELECT * FROM  standard1 WHERE roll_no = " + to_string(roll_no) + "AND  standard = " + to_string(standard));
                    if (!(res.empty()))
                    {
                        cout << "Roll no exists" << endl;
                    }
                    else
                    {
                        break;
                    }
                }
                while (1)
                {
                    cout << "Enter the student name: ";
                    cin >> name;
                    if (Sf.studentname_validation(name) == 1)
                    {
                        student->name = name;
                        break;
                    }
                    else
                    {
                        cout << "Invalid name" << endl;
                    }
                }
                student->AddSubjectSpecific();
            }
            else if (choice == 2)
            {
                student->GetDetails();
            }
            else if (choice == 3)
            {
                student->DeleteStudent();
            }
            else if (choice == 4)
            {
                student->max_marks();
            }
            else if (choice == 5)
            {
                student->min_marks();
            }
            else if (choice == 6)
            {
                student->search_by_character();
            }
            else if (choice == 7)
            {
                student->modify_details();
            }
            else if (choice == 0)
            {
                exit(0);
            }
            else
            {
                cout << "Invalid choice. Please try again." << std::endl;
            }
        }

        else if (standard == 4)
        {
            while (true)
            {
                int choice;
                cout << endl <<"****************STUDENT MENU****************" << endl;
                cout << "1. Add details\n2. Get details\n3. Delete student\n4. Highest Marks\n5. Lowest Marks\n6. Search_by_character\n7. Modify student\n0. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice;
                if (choice == 1)
                {
                    while (1)
                    {
                        cout << "Enter Standard: ";
                        cin >> standard;
                        if (Sf.studentstandard_validation(standard) == 1)
                            break;
                        else
                            cout << "Invalid Standard" << endl;
                    }
                    while (1)
                    {
                        cout << "Enter roll_no: ";
                        cin >> roll_no;
                        pqxx::work txn(conn);
                        pqxx::result res = txn.exec("SELECT * FROM  standard1 WHERE roll_no = " + to_string(roll_no) + "AND  standard = " + to_string(standard));
                        if (!(res.empty()))
                        {
                            cout << "Roll no exists" << endl;
                        }
                        else
                        {
                            break;
                        }
                    }
                    while (1)
                    {
                        cout << "Enter the student name: ";
                        cin >> name;
                        if (Sf.studentname_validation(name) == 1)
                        {
                            student->name = name;
                            break;
                        }
                        else
                        {
                            cout << "Invalid name" << endl;
                        }
                    }
                    student->AddSubjectSpecific();
                }
                else if (choice == 2)
                {
                    student->GetDetails();
                }
                else if (choice == 3)
                {
                    student->DeleteStudent();
                }
                else if (choice == 4)
                {
                    student->max_marks();
                }
                else if (choice == 5)
                {
                    student->min_marks();
                }
                else if (choice == 6)
                {
                    student->search_by_character();
                }
                else if (choice == 7)
                {
                    student->modify_details();
                }
                else if (choice == 0)
                {
                    exit(0);
                }
                else
                {
                    cout << "Invalid choice. Please try again." << std::endl;
                }
            }
        }

        else if (standard == 5)
        {
            break;
        }
        else
        {
            cout << "Invalid choice. Please try again." << std::endl;
        }
    }
    delete student;
    return 0;
}
