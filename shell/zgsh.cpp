#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <cstring>
#include <string>
#include <numeric>
#include <fcntl.h>
#include "start_animation.hpp"
using namespace std;
string order;
vector<string> segcmd;
vector<char *> args[100];
char path[1024];
bool cdcmd = false, htpro = false;
int pipecount, pipes[100][2], argscount;
pid_t pids[100];
char *last_time_path = new char[100];
void getcurrentdir()
{
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
        perror("getcwd");
    }
}
void erase_args(int count, int index)
{
    delete[] args[count][index];
    args[count].erase(args[count].begin() + index);
}
void redirect(int count)
{
    for (int i = 0; (int)i < args[count].size(); i++)
    {
        if (args[count][i] == nullptr)
        {
            i++;
            continue;
        }
        if (strcmp(args[count][i], "<") == 0)
        {
            int fd = open(args[count][i + 1], O_RDONLY, 0644);
            dup2(fd, STDIN_FILENO);
            close(fd);
            erase_args(count, i + 1);
            erase_args(count, i);
            continue;
        }
        else if (strcmp(args[count][i], ">") == 0)
        {
            int fd = open(args[count][i + 1], O_RDWR | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            erase_args(count, i + 1);
            erase_args(count, i);
            continue;
        }
        else if (strcmp(args[count][i], "2>") == 0)
        {
            int fd = open(args[count][i + 1], O_RDWR | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDERR_FILENO);
            close(fd);
            erase_args(count, i + 1);
            erase_args(count, i);
            continue;
        }
        else if (strcmp(args[count][i], ">>") == 0)
        {
            int fd = open(args[count][i + 1], O_RDWR | O_APPEND | O_CREAT, 0644);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            erase_args(count, i + 1);
            erase_args(count, i);
            continue;
        }
    }
}
void splitcmd()
{
    string a;
    size_t start = 0;
    for (int i = 0; i < (int)order.size(); i++)
    {
        if (order[i] == '|' || i == (int)order.size() - 1)
        {
            if (i == order.size() - 1)
            {
                a = order.substr(start);
                char *arg = new char[a.size() + 1];
                strcpy(arg, a.c_str());
                segcmd.push_back(arg);
                delete[] arg;
            }
            else
            {
                a = order.substr(start, i - start - 1);
                char *arg = new char[a.size() + 1];
                strcpy(arg, a.c_str());
                segcmd.push_back(arg);
                delete[] arg;
            }
            start = i + 2;
        }
    }
}
void segstr(int count, string &a)
{
    args[count].clear();
    int start = 0, end = 0;
    while (end <= a.size())
    {
        if (a[end] == ' ' || end == a.size() - 1)
        {
            string b;
            if (end == a.size() - 1)
            {
                b = a.substr(start);
            }
            else
            {
                b = a.substr(start, end - start);
            }
            char *arg = new char[a.size() + 1];
            strcpy(arg, b.c_str());
            args[count].push_back(arg);
            if (end != a.size())
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
    if (order.size() == 2)
    {
        size_t cdpos = order.find("cd");
        order.insert(cdpos + 2, " /home/zgyx");
    }
    string broken = order.substr(3);
    if (broken == "-")
    {
        cout << last_time_path << endl;
        if (chdir(last_time_path) == -1)
        {
            perror("cd");
        }
    }
    else
    {
        memset(last_time_path, '\0', sizeof(last_time_path));
        char* name = getcwd(path, sizeof(path));
        strcpy(last_time_path, name);
        if (chdir(broken.c_str()) == -1)
        {
            cout << "cd: 没有那个文件或目录或参数太多" << endl;
        }
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
        redirect(count);
        for (int i = 0; i < pipecount; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        if (execvp(args[count][0], args[count].data()) == -1 && cdcmd == false)
        {
            cout << "zgsh: command not found: " << args[count][0] << endl;
            for (int i = 0; i < argscount; ++i)
            {
                for (char *ptr : args[i])
                {
                    delete[] ptr;
                }
                args[i].clear();
            }
            exit(1);
        }
    }
    else if (pid > 0 && htpro)
    {
        if (count == 0)
        {
            cout << "[" << count + 1 << "]";
        }
        cout << " " << pid;
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
    argscount = segcmd.size();
    pipecount = argscount - 1;
    for (int j = 0; j < pipecount; j++)
    {
        if (pipe(pipes[j]) == -1)
        {
            perror("pipe");
            exit(1);
        }
    }
}
void sign()
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
}
int main()
{
    print();
    sign();
    while (start)
    {
        for (auto &v : args)
        {
            v.clear();
        }
        getcurrentdir();
        getline(cin, order);
        space_kg();
        if (order.empty())
        {
            continue;
        }
        else if (order.find("clear") != string::npos)
        {
            system("clear");
            continue;
        }
        else if (order.find("ls") != string::npos)
        {
            lscolor();
        }
        else if (order.find("cd") != string::npos)
        {
            cdcommit();
            continue;
        }
        else if (order == "exit" || order == "eee")
        {
            delete[] last_time_path;
            break;
        }
        if (order.find("&") != string::npos)
        {
            size_t hpos = order.find("&");
            order = order.substr(0, hpos);
            htpro = true;
            space_kg();
        }
        splitcmd();
        findpipe();
        for (int i = 0; i < argscount; i++)
        {
            string a = accumulate(segcmd[i].begin(), segcmd[i].end(), string());
            segstr(i, a);
            pidfork(fork(), i);
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
        if (htpro)
        {
            cout << endl;
        }
        for (int i = 0; i < argscount; ++i)
        {
            for (char *ptr : args[i])
            {
                delete[] ptr;
            }
            args[i].clear();
        }
        memset(pipes, 0, sizeof(pipes));
        order.clear();
        segcmd.clear();
        argscount = pipecount = 0;
    }
}