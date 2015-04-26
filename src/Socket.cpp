#include "Socket.h"


Socket::Socket() : m_nSocketID(-1)
{
    memset(&m_SocketAddress, 0, sizeof(m_SocketAddress));
}

Socket::~Socket()
{
    if (IsValid())
#if defined(_WIN32)
        closesocket(m_nSocketID);
#else
        close(m_nSocketID);
#endif // defined

}

bool Socket::Create()
{
#if defined(_WIN32)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        return false;
#endif

    //m_nSocketID = socket(AF_INET, SOCK_STREAM, 0);
    //m_nSocketID = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    m_nSocketID = socket(PF_INET, SOCK_STREAM, 0);

    if (!IsValid())
        return false;

    int yes = 1;
    if (setsockopt(m_nSocketID, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(int)) == -1 )
        return false;

    return true;
}

bool Socket::Bind(const int port)
{
    if (!IsValid())
        return false;

    m_SocketAddress.sin_family = AF_INET;
    m_SocketAddress.sin_port = htons(port);
    m_SocketAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_nSocketID, (struct sockaddr *)&m_SocketAddress, sizeof(m_SocketAddress)) == -1)
        return false;

  return true;
}

bool Socket::Listen() const
{
    if (!IsValid())
        return false;

    //int nListenReturn = ::listen( m_nSock, MAXCONNECTIONS);
    if (listen(m_nSocketID, BACKLOG) == -1)
        return false;

    return true;
}
/*
bool Socket::Accept(Socket &cSocket) const
{
    int nAddrlength = sizeof(m_SocketAddress);
    //NewSocket.m_nSock = ::accept(m_nSock, (sockaddr *)&m_Addr, (socklen_t *)&nAddrlength);
    cSocket.m_nSocketID = accept(m_nSocketID, (sockaddr *)&m_SocketAddress, (socklen_t *)&nAddrlength); // TODO: Is second parameter correct?

    if (cSocket.m_nSocketID == -1)
        return false;
    //else

    return true;
}
*/

bool Socket::Accept()
{
    m_nSendRecvSocketID = accept(m_nSocketID, (sockaddr *) NULL, NULL);

    if (m_nSendRecvSocketID == -1)
        return false;

    return true;
}

bool Socket::Send(const std::string sMessage) const
{
    if (send(m_nSendRecvSocketID, sMessage.c_str(), sMessage.size(), 0) == -1)
        return false;

    return true;
}

int Socket::Recv(std::string& sMessage) const
{
    char cBuf[MAXRECV + 1];
    memset(cBuf, 0, MAXRECV + 1);

    sMessage = ""; // TODO: Why not use cBuf and get rid of sMessage?

    int nStatus = recv(m_nSendRecvSocketID, cBuf, MAXRECV, 0);

    if (nStatus == -1)
    {
        std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
        return 0;
    }
    else if (nStatus == 0)
    {
        return 0;
    }
    else
    {
        sMessage = cBuf;
        return nStatus;
    }
}

bool Socket::Connect(const std::string sHost, const int nPort)
{
    // getaddrinfo variables
    //struct addrinfo hints, *result;
    //memset(&hints, 0, sizeof(hints));
    //hints.ai_family = AF_INET;
    //hints.ai_socktype = SOCK_STREAM;

    //if (getaddrinfo(sHost.c_str(), NULL, &hints, &result) != 0)
    //    return false;

    //struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;
    //m_SocketAddress.sin_addr = ipv4->sin_addr;

    if (!IsValid())
        return false;

    m_SocketAddress.sin_family = AF_INET;
    m_SocketAddress.sin_port = htons(nPort);

    m_SocketAddress.sin_addr.s_addr = inet_addr(sHost.c_str());


    //if (inet_pton(AF_INET, sHost.c_str(), &m_SocketAddress.sin_addr) != 1)
    //    return false;

    //if (errno == EAFNOSUPPORT)
    //    return false;

    //nStatus = ::connect (m_nSock, (sockaddr *)&m_Addr, sizeof(m_Addr));
    //nStatus = connect(m_nSocketID, (sockaddr *)&m_SocketAddress, sizeof(m_SocketAddress));

    /*
    do
    {
        if (connect(m_nSocketID, (sockaddr *)&m_SocketAddress, sizeof(m_SocketAddress)) == -1)
        {
            if (WSAGetLastError() != WSAECONNREFUSED)
                return false;
        }
    } while (true);
    */

    if (connect(m_nSocketID, (sockaddr *)&m_SocketAddress, sizeof(m_SocketAddress)) == -1)
    {
#if defined(_WIN32)
        std::cerr << "Error when connecting " << WSAGetLastError() << std::endl;
#else
        perror("connect");
#endif // defined
        return false;
    }

    //if (nStatus == 0)
    //    return true;
    //else
        //return false;

    m_nSendRecvSocketID = m_nSocketID;

    return true;
}

/*
void Socket::SetNonBlocking(const bool b )
{
    int nFlags = fcntl(m_nSocketID, F_GETFL);

    if (nFlags == -1)
        return;

    if (b)
        nFlags = (nFlags | O_NONBLOCK);
    else
        nFlags = (nFlags & ~O_NONBLOCK);

    fcntl(m_nSocketID, F_SETFL, nFlags);
}
*/
