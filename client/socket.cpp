#include "socket.h"
#include "iostream"

namespace network {
udp_socket::udp_socket(const QHostAddress &host,
                       quint16 port,
                       const std::string &type,
                       QObject *parent) : type(type) {
    socket = new QUdpSocket(this);
    socket->bind(host, port);
    qDebug() << "Server is running at:" << host << "and port is:" << port;
    connect(socket, &QUdpSocket::readyRead, this,
            &udp_socket::readPendingDatagrams);
}

std::vector<std::string> udp_socket::parse(const std::string &data) {
    std::vector<std::string> parsed;
    std::istringstream raw_query(data);
    std::string token;
    while (std::getline(raw_query, token, ',')) {
        parsed.push_back(token);
    }
    if (parsed.back().back() == '\n') {
        parsed.back().pop_back();  // removing leading '\n'
    }
    std::cout << parsed[0] << std::endl;
    return parsed;
}

void udp_socket::send_datagram(const std::string &data, const client &to) {
    QByteArray datagram(data.c_str());
    socket->writeDatagram(datagram, to.address, (type == "server") ? 60000 : to.port);
    socket->waitForReadyRead(500);
}

void udp_socket::readPendingDatagrams() {
    QNetworkDatagram datagram;
    while (socket->hasPendingDatagrams()) {
        datagram = socket->receiveDatagram();
        qDebug() << "Received new message from"
                 << datagram.senderAddress().toString() << "at"
                 << datagram.senderPort();
        queries.push({datagram.data().toStdString(),
                      {datagram.senderAddress(), datagram.senderPort()}});
    }
    process();
}
}  // namespace network