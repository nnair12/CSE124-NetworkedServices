
namespace py tritonTransfer

typedef string fileName
typedef list<string> hashlist

service transfer {
    string registerBlockServer(1:i32 port),

    list<string> uploadFile(1:fileName file, 2:hashlist hashes),
    string uploadBlock(1:string hash, 2:binary block),

    list<string> downloadFile(1:string fileName)
    string downloadBlock(1:string hash)
}
