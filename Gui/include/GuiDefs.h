#pragma once

#include "Client.h"
#include "commonDefs.h"

#include <QIcon>
#include <QMessageBox>

enum MessageIndex { WARNING, CRITICAL, INFORMATION, QUESTION };

inline int Message(MessageIndex index, std::string message, std::string source,
                   std::string customTitle = "") {

    // replace all \n with <br>
    size_t pos = 0;
    while ((pos = message.find("\n", pos)) != std::string::npos) {
        message.replace(pos, 1, "<br>");
        pos += 1;
    }

    // add source in gray
    message.append(
        std::string(
            "<p style=\"font-size:12px;color:grey;\">Source:&nbsp;&nbsp; ") +
        source + std::string("</p>"));

    QMessageBox::Icon icon = QMessageBox::Question;
    std::string title;
    switch (index) {
    case WARNING:
        icon = QMessageBox::Warning;
        title = "WARNING";
        break;
    case CRITICAL:
        icon = QMessageBox::Critical;
        title = "CRITICAL";
        break;
    case INFORMATION:
        icon = QMessageBox::Information;
        title = "INFORMATION";
        break;
    default:
        icon = QMessageBox::Question;
        title = "QUESTION";
        break;
    }
    if (!customTitle.empty()) {
        title = customTitle;
    }
    int ret = OK;
    if (icon == QMessageBox::Question) {
        QMessageBox msgBox(icon, title.c_str(), message.c_str(),
                           QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setWindowIcon(QIcon(":/icons/images/motorIcon.png"));
        ret = msgBox.exec();
    } else {
        QMessageBox msgBox(icon, title.c_str(), message.c_str(),
                           QMessageBox::Ok);
        msgBox.setWindowIcon(QIcon(":/icons/images/motorIcon.png"));
        ret = msgBox.exec();
    }
    if (ret == QMessageBox::Ok) {
        return OK;
    }
    return FAIL;
}

inline int getInteger(std::string result) {
    int value = 0;
    std::istringstream iss(result.c_str());
    iss >> value;
    if (iss.fail()) {
        throw std::runtime_error("Cannot scan int from " + result);
    }
    return value;
}

inline double getDouble(std::string result) {
    double value = 0;
    std::istringstream iss(result.c_str());
    iss >> value;
    if (iss.fail()) {
        throw std::runtime_error("Cannot scan double from " + result);
    }
    return value;
}

inline std::string SendCommand(std::string hostname, int nCommand,
                               std::string command, std::string source) {
    std::string result;

    try {
        Client client(hostname);
        result = client.SendCommand(nCommand + 1, "gui " + command);
    }

    catch (const AnotherUserError &e) {
        Message(WARNING, e.what(), source);
    }

    catch (const BoxInUseError &e) {
        std::ostringstream oss;
        oss << e.what() << "\nUnlock previous user?";
        // abort
        if (FAIL == Message(QUESTION, oss.str(), source, "Lock GUI?")) {
            throw std::runtime_error(
                "You have chosen to let other user continue.");
        }
        // unlock other user
        else {
            try {
                Client client(hostname);
                std::string res = client.SendCommand(2, "gui unlock");
                res += "\nPlease update your gui!";
                Message(INFORMATION, res, source);
            } catch (const std::exception &e) {
                Message(WARNING, e.what(), source);
            }
        }
    }

    catch (const TubeOffError &e) {
        throw;
    }

    catch (const TubeStandbyError &e) {
        throw;
    }

    catch (const PressureOffError &e) {
        throw;
    }

    catch (const std::exception &e) {
        Message(WARNING, e.what(), source);
    }
    return result;
}
