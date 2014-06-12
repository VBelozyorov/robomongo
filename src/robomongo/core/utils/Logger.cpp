#include "robomongo/core/utils/Logger.h"

#include <QDir>
#include <QMetaType>
#include <QStandardPaths>

#include "robomongo/core/utils/QtUtils.h"

namespace
{

    std::string getLoggerPath()
    {
        // @todo Qt4 approach, wasn't tested
//        static std::string path = Robomongo::QtUtils::toStdString(
//            QString("%1/"PROJECT_NAME_LOWERCASE".log").arg(QDesktopServices::storageLocation(QDesktopServices::CacheLocation))
//        );

        QStringList locations = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
        if (locations.isEmpty()) {
            // @todo Should we write logs in location with potentially common access or don't write it at all?
            return std::string("", 0);
        }
        else {
            // @todo If locations[0] won't be writable for some reason, logs won't be written
            static std::string path = Robomongo::QtUtils::toStdString(
                QString("%1/"PROJECT_NAME_LOWERCASE".log").arg(locations[0])
            );
            return path;
        }
    }
}

namespace Robomongo
{
    Logger::Logger()
    {
        qRegisterMetaType<mongo::LogLevel>("mongo::LogLevel");
        std::string path = getLoggerPath();
        QFile file(QtUtils::toQString(path)); //delete file if it size more than 5mb
        if (file.exists() && file.size() > 5 * 1024 * 1024) {
            file.remove();
        } 
        else {
            QFileInfo fileInfo(file);
            QString filePath = fileInfo.path();
            QDir fileDir;
            fileDir.mkpath(filePath);
        }
        mongo::initLogging(path,true);
    }

    Logger::~Logger()
    {   
    }

    void Logger::print(const char *mess, mongo::LogLevel level, bool notify)
    {
        print(std::string(mess), level, notify);
    }

    void Logger::print(const std::string &mess, mongo::LogLevel level, bool notify)
    {       
        print(QtUtils::toQString(mess), level, notify);
    }

    void Logger::print(const QString &mess, mongo::LogLevel level, bool notify)
    {        
        LOG(level) << "["PROJECT_NAME_TITLE"] " << QtUtils::toStdString(mess) << std::endl;
        if (notify)
            emit printed(mess, level);
    }
}
