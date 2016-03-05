
namespace py tritonTransfer

typedef string fileName
typedef list<string> hashlist

service transfer {
    list<string> uploadFile(1:fileName file, 2:hashlist hashes),
    string uploadBlock(1:string hash, 2:binary block),

    list<string> downloadFile(1:string fileName)
    string downloadBlock(1:string hash)
}
