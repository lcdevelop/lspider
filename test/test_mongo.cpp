/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Tue 19 May 2015 02:27:33 PM CST
 * Description: 
 ************************************************************************/

#include "mongo/client/dbclient.h"

using namespace mongo;
using namespace std;

int main(int argc, char *argv[])
{
    DBClientConnection conn;
    try {

        // 连接
        conn.connect("localhost:27017");

        // 查询sql
        auto_ptr<DBClientCursor> cursor = conn.query("mydb.user");
        while ( cursor->more() ) {
            BSONObj obj = cursor->next();
            if( !obj.isEmpty() ) {
                printf("%s\n", obj.toString().c_str());
            }
        }

        // 查询指定id
        BSONObj obj = conn.findOne( "mydb.user", BSONObjBuilder().append( "_id" , "1234" ).obj()); 
        if( !obj.isEmpty() ){
            string value = obj.getStringField("value");
            printf("findOne value=%s\n", value.c_str());
        }

        // 删除
        conn.remove( "mydb.user", BSONObjBuilder().append( "_id" , "1234" ).obj()); 

        // 插入
        mongo::BSONObjBuilder b;
        b.append( "_id", "1234" );
        b.append( "value", "hello" );
        conn.update( "mydb.user", BSONObjBuilder().append( "_id" , "1234" ).obj(), b.obj(), true );
    } catch (DBException &e) {
        printf("%s\n", e.what());
    } catch ( std::exception& e ) {  
        printf("MONGO Exception(set): %s", e.what());
        return -1;
    } catch( ... ){
        printf("MONGO Exception(set): NULL");
        return -1;
    }
    return 0;
}
