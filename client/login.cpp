#include "login.h"
#include "ui_login.h"
#include <QLineEdit>
#include <QMetaType>
#include "client_socket.h"
#include "main_window.h"
#include "user.h"

extern network::client server;
extern user current_user;
extern client_processor processor;

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    connect(&registration_m, SIGNAL(show_login_window_again()), this, SLOT(show_login_window()));

    qRegisterMetaType<std::string>("std::string");
    connect(&processor, SIGNAL(run_successful_login(std::string)), this, SLOT(successful_login(std::string)));
    connect(&processor, SIGNAL(run_wrong_password()), this, SLOT(wrong_password()));
    connect(&processor, SIGNAL(run_no_user()), this, SLOT(no_user()));
    connect(&processor, SIGNAL(run_error()), this, SLOT(error()));
    ui->setupUi(this);
    this->setWindowTitle("ChitChat");
}

login::~login() {
    delete ui;
}

void login::on_show_password_check_box_stateChanged(int arg1) {
    if (arg1) {
        ui->password_line_edit->QLineEdit::setEchoMode(QLineEdit::Normal);
    } else {
        ui->password_line_edit->QLineEdit::setEchoMode(QLineEdit::Password);
    }
}

void login::on_log_in_button_clicked() {
    std::string login, password;
    login = (ui->login_line_edit->text().toStdString());
    password = ui->password_line_edit->text().toStdString();
    processor.prepare_query("login," + login + "," + password, server);
}

void login::on_create_new_account_button_clicked()
{
    this->hide();
    registration_m.show();
}

void login::show_login_window() {
    this->show();
}

void login::successful_login(const std::string& name){
    current_user.set_name(name);
    this->hide();
    emit show_main_window();
}

void login::wrong_password(){
    ui->information_label->setText("Wrong password");
}

void login::no_user(){
    ui->information_label->setText("User is not found");
}

void login::error(){
    ui->information_label->setText("Something go wrong");
};
