#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <cstring>
#include <string>
using namespace std;
string order;
vector<char *> args[100];
char path[1024];
char last_time_path[1024];
bool cdcmd = false;
int pipecount, pipes[100][2], argscount;
pid_t pids[100];
void getcurrentdir()
{
    memset(pipes, 0, sizeof(pipes));
    order.clear();
    argscount = pipecount = 0;
    for (auto &v : args)
    {
        v.clear();
    }
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
void segstr(int count)
{
    args[count].clear();
    int start = 0, end = 0;
    while (end <= order.size())
    {
        if (order[end] == '|')
        {
            size_t pipepos = order.find_first_of('|');
            order = order.substr(pipepos + 2);
            break;
        }
        else if (order[end] == ' ' || end == order.size())
        {
            string a = order.substr(start, end - start);
            char *arg = new char[a.size() + 1];
            strcpy(arg, a.c_str());
            args[count].push_back(arg);
            if (end != order.size())
            {
                start = end + 1;
            }
        }
        end++;
    }
    args[count].push_back(nullptr);
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
void pidfork(pid_t pid, int count)
{
    pids[count] = pid;
    if (pid < 0)
    {
        cout << "Fork failed!" << endl;
        exit(1);
    }
    else if (pids[count] == 0)
    {
        if (count > 0)
        {
            dup2(pipes[count - 1][0], STDIN_FILENO);
        }
        if (count < pipecount)
        {
            dup2(pipes[count][1], STDOUT_FILENO);
        }
        for (int i = 0; i < pipecount; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        if (execvp(args[count][0], args[count].data()) == -1 && cdcmd == false)
        {
            cout << "zgsh: command not found: " << args[count][0] << endl;
        }
        exit(1);
    }
    for (int j = 0; j < pipecount; j++)
    {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }
    for (int i = 0; i < argscount; i++)
    {
        waitpid(pids[i], NULL, 0);
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
    for (char c : order)
    {
        if (c == '|')
        {
            pipecount++;
        }
    }
    argscount = pipecount + 1;
    for (int j = 0; j < pipecount; j++)
    {
        if (pipe(pipes[j]) == -1)
        {
            perror("pipe");
            exit(1);
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
        if (order.empty())
        {
            continue;
        }
        else if (order.find("clear") != std::string::npos)
        {
            system("clear");
            continue;
        }
        else if (order.find("ls") != std::string::npos)
        {
            lscolor();
        }
        else if (order.find("cd") != std::string::npos)
        {
            cdcommit();
            continue;
        }
        else if (order == "exit")
        {
            break;
        }
        findpipe();
        for (int i = 0; i < argscount; i++)
        {
            segstr(i);
            pidfork(fork(), i);
        }
        for (int i = 0; i < argscount; ++i)
        {
            for (char *ptr : args[i])
            {
                delete[] ptr;
            }
            args[i].clear();
        }
    }
}