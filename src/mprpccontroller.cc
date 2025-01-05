#include"mprpccontroller.h"

MprpcController::MprpcController()
{
    _failed=false;
    _errtext="";
}
void MprpcController::Reset()
{
    _failed=false;
    _errtext="";
}
bool MprpcController::Failed() const
{
    return _failed;
}
std::string MprpcController::ErrorText() const
{
    return _errtext;
}
void MprpcController::SetFailed(const std::string& reason)
{
    _failed=true;
    _errtext=reason;
}

void MprpcController::StartCancel(){}
bool MprpcController::IsCanceled() const {return false;}
void MprpcController::NotifyOnCancel(google::protobuf::Closure* callback){}