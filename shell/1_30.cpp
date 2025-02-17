#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <cstring>
#include <string>
using namespace std;
string order, filename;
vector<char *> args;
char path[1024];
char last_time_path[1024];
bool cdcmd = false;
int pipecount, pipes[100][2];
void getcurrentdir()
{
    order.clear();
    filename.clear();
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
void getfilename()
{
    size_t namepos = order.find_first_of(' ');
    filename = order.substr(0, namepos);
}
void cdcommit()
{
    cdcmd = true;
    strcpy(last_time_path, path);
    if (order.size() == 2)
    {
        size_t cdpos = order.find("cd");
        order.insert(cdpos + 2, " /home/zgyx");
    }
    string broken = order.substr(3);
    if (broken == "-")
    {
        cout << last_time_path << endl;
    }
    else if (chdir(broken.c_str()) == -1)
    {
        cout << "cd: 没有那个文件或目录或参数太多" << endl;
    }
}
void pidfork(pid_t pid)
{
    if (pid < 0)
    {
        cout << "Fork failed!" << endl;
        exit(1);
    }
    else if (pid == 0)
    {
        if (execvp(filename.c_str(), args.data()) == -1 && cdcmd == false)
        {
            cout << "zgsh: command not found: " << order << endl;
        }
        exit(1);
    }
    else
    {
        wait(nullptr);
    }
}
void space_kg(int strindex = 0)
{
    while (strindex <= order.size())
    {
        if ((order[strindex] == ' ' && order[strindex + 1] == ' ') || (order[strindex] == ' ' && strindex == order.size() - 1) || (order[strindex] == ' ' && strindex == 0))
        {
            order.erase(strindex, 1);
        }
        else
        {
            strindex++;
        }
    }
}
void lscolor()
{
    size_t lspos = order.find("ls");
    order.insert(lspos + 2, " --color=auto");
}
void findpipe()
{
    for (int i = 0; i < order.size(); i++)
    {
        if (order[i] == '|')
        {
            pipecount++;
        }
    }
}
int main()
{
    while (1)
    {
        getcurrentdir();
        getline(cin, order);
        space_kg();
        findpipe();
        if (order.empty())
        {
            continue;
        }
        else if (order.find("clear") != std::string::npos)
        {
            system("clear");
        }
        else if (order.find("ls") != std::string::npos)
        {
            lscolor();
        }
        else if (order.find("cd") != std::string::npos)
        {
            cdcommit();
        }
        getfilename();
        segstr();
        pidfork(fork());
    }
}