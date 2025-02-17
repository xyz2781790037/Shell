#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <cstring>
using namespace std;
string order;
vector<char *> args;
int count = 0;
void getcurrentdir()
{
    char path[1024];
    if (getcwd(path, sizeof(path)) != nullptr)
    {
        cout << "\033[32m➜  \033[0m";
        string cwd;
        cwd.assign(path);
        size_t pathpos = cwd.find_last_of('/');
        string m_Name = cwd.substr(pathpos + 1);
        cout << "\033[1;36m" << m_Name << "\033[0m" << " ";
    }
    else
    {
        cerr << "Error getting current directory!" << endl;
    }
}
void segstr()
{
    args.clear();
    int start = 0, end = 0;
    while (end <= order.size())
    {
        if (order[end] == ' ' || end == order.size())
        {
            string a = order.substr(start, end - start);
            char *arg = new char[a.size() + 1];
            strcpy(arg, a.c_str());
            args.push_back(arg);
            if (end != order.size())
            {
                start = end + 1;
            }
        }
        end++;
    }
    args.push_back(nullptr);
}
int clearcmd()
{
    if (order == "clear")
    {
        return 1;
    }
    return 0;
}
int lscolor()
{
    if (order[0] == 'l' && order[1] == 's')
    {
        return 1;
    }
    return 0;
}
int main()
{
    string filename;
    while (1)
    {
        getcurrentdir();
        getline(cin, order);
        if (order.empty())
        {
            continue;
        }
        else if (clearcmd())
        {
            system("clear");
        }
        else if (lscolor)
        {
            order += " --color=auto";
        }
        for (char s : order)
        {
            if (s == ' ')
            {
                break;
            }
            filename += s;
        }
        segstr();
        pid_t pid = fork();
        if (pid < 0)
        {
            cout << "Fork failed!" << endl;
            return 1;
        }
        else if (pid == 0)
        {
            if (execvp(filename.c_str(), args.data()) == -1)
            {
                perror("execvp");
            }
            exit(1);
        }
        else
        {
            wait(nullptr);
        }
        order.clear();
        filename.clear();
    }
}