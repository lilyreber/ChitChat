#include "server.h"
#include <cassert>
#include "state.h"

namespace sv {

void server_processor::process() {
    while (!keeper->parsed_queries.empty()) {
        auto query = keeper->parsed_queries.front();
        keeper->parsed_queries.pop();
        data = parse(query.first);
        to = query.second;
        try {
            switch (commands.at(data[0])) {
                case e_commands::LOGIN:
                    authorize_user();
                    break;
                case e_commands::REGISTER:
                    register_user();
                    break;
                case e_commands::CONNECT:
                    connect_user();
                    break;
                case e_commands::GREET:
                    greet();
                    break;
                case e_commands::MOVE:
                    update_layout();
                    break;
                case e_commands::DISCONNECT:
                    disconnect();
                    break;
            }
        } catch (std::out_of_range &e) {
            prepare_query("Unknown command " + data[0] + "\n", query.second);
        }
    }
}

void server_processor::authorize_user() {
    assert(data.size() == 3);
    try {
        if (model::database::authorize(data[1], data[2])) {
            prepare_query("allowed," + data[1] + "\n", to);
        } else {
            prepare_query("denied," + data[1] + "\n", to);
        }
    } catch (model::no_user_found &) {
        prepare_query("none," + data[1] + "\n", to);
    } catch (model::database_error &) {
        prepare_query("dberror\n", to);
    }
}

void server_processor::register_user() {
    assert(data.size() == 3);
    user new_user(data[1], data[2]);
    if (model::database::create_user(&new_user)) {
        prepare_query("created," + data[1] + "\n", to);
    } else {
        prepare_query("rexists," + data[1] + "\n", to);
    }
}

void server_processor::connect_user() {
    assert(data.size() == 3);
    server_user new_user{data[1], data[2], to};
    if (model::state::connect_user(new_user)) {
        translate_users_data();
        new_user_connected();
    } else {
        prepare_query("cexists," + data[1] + "\n", to);
    }
}

void server_processor::greet() {
    assert(data.size() == 2);
    prepare_query("Hello, " + data[1] + ", I'm Server God!\n", to);
}

void server_processor::update_layout() {
    for (auto &i : data) {
        qDebug() << i.c_str() << "Same request";
    }
    qDebug() << "======== ASSERTION HERE ========";
    assert(data.size() == 4);
    qDebug() << "entered in move";
    model::state::update_coords(data[1], std::stoi(data[2]),
                                std::stoi(data[3]));
    for (const auto &u : model::state::get_users()) {
        if (u.client != to) {
            prepare_query(
                "move," + data[1] + "," + data[2] + "," + data[3] + "\n",
                u.client);
        }
    }
}

void server_processor::translate_users_data() {
    std::string all_users = "connected,";
    // TODO
    for (const auto &u : model::state::get_users()) {
        all_users += u.name() + "," + std::to_string(u.get_coords().x) + "," +
                     std::to_string(u.get_coords().y) + ",";
    }
    if (all_users.back() == ',')
        all_users.pop_back();
    if (!all_users.empty())
        all_users += "\n";
    prepare_query(all_users, to);
}

void server_processor::disconnect() {
    assert(data.size() == 5);
    model::state::disconnect_user(server_user(
        data[1], data[2], to, std::stoi(data[3]), std::stoi(data[4])));
    for (const auto &u : model::state::get_users()) {
        if (u.name() != data[1]) {
            prepare_query("disconnected," + data[1] + "\n", u.client);
        }
    }
}
server_processor::server_processor(network::queries_keeper *pKeeper,
                                   network::tcp_socket &socket)
    : query_processor(pKeeper, socket) {
}

void server_processor::new_user_connected() {
    for (const auto &u : model::state::get_users()) {
        if (u.name() != data[1]) {
            prepare_query("new," + data[1] + "\n", u.client);
        }
    }
}

}  // namespace sv