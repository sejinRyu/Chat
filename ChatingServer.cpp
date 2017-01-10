#include "TcpSocket.h"
#include <mysql++/mysql++.h>
#include <mysql++/result.h>
#include <signal.h>
#include <vector>

using namespace std;

const int key=23;

void childHandler(int tmp)
{
    wait(0);
}


int main(int argc,char** argv)  //
{
    signal(SIGCHLD,childHandler);
    try
    {
        ServerTcpSocket server(9999);
        while(server.accept()>0)
        {
            pid_t pid;
            if((pid=fork())==0)
            {
                cout<<"connect!"<<endl;
                string userID=server.receive(23);
                int roomNumber=atoi(server.receive(23).c_str());
        
                mysqlpp::Connection con("chatroom","10.156.145.48","root","shangus1",3306);
                mysqlpp::Query query=con.query();
                mysqlpp::StoreQueryResult result;

                //error
                /*query<<"select NAME from room"+to_string(roomNumber)+" where ID="+userID;
                result=query.store();
                string userName=result.at(0)["NAME"];*/

                {//text receive
                    string msg=server.receive(key);
                    query<<"insert into room"+to_string(roomNumber)+"(sender,msg) values('"+userID+"','"+msg+"')";
                    query.store();
                }
                {//text alarm
                    int nowmsg;
                    query<<"select nowmsg from chatserver.roomnumber";
                    result=query.store();
                    nowmsg=result.at(0)["nowmsg"];
                    while(1)
                    {
                        query<<"select nowmsg from chatserver.roomnumber";
                        result=query.store();
                        if(nowmsg!=atoi(result.at(0)["nowmsg"]))
                        {
                            nowmsg++;
                            query<<"select * from room"+to_string(roomNumber)+" where number='"+to_string(nowmsg)+"'";
                            result=query.store();
                            if(result.at(0)["sender"]!=userID)
                            {
                                server.send(string(result.at(0)["date"]),key);
                                server.send(string(result.at(0)["sender"]),key);
                                server.send(string(result.at(0)["msg"]),key);
                            }
                        }
                    }


                }
                
                
            }
        }
    } 
    catch (mysqlpp::BadQuery& e) 
    {
        cerr << "BadQuery : " << e.what() << endl;
        cerr<<e.errnum()<<endl;
    }
    catch(const char* e)
    {
        perror(e);
    }
    return 0;
}