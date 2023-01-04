#ifndef DOWORK_H
#define DOWORK_H

#include "actionworker.h"
#include "doworker.h"

class doWork
{
public:
    doWork();

    static ActionResult get_data(const QUrlQuery& param, const QByteArray& content);
    static ActionResult get_batt(const QUrlQuery& param, const QByteArray& content);
    static ActionResult active(const QUrlQuery& param, const QByteArray& content);
    static ActionResult version(const QUrlQuery &param, const QByteArray &content);
    static ActionResult version4(const QUrlQuery &param, const QByteArray &content);
    static ActionResult version5(const QUrlQuery &param, const QByteArray &content);
    static ActionResult instance(const QUrlQuery &param, const QByteArray &content);
    static ActionResult hwinfo(const QUrlQuery &param, const QByteArray &content);
    static ActionResult get_data_length(const QUrlQuery &param, const QByteArray &content);
    static ActionResult get_storage_status(const QUrlQuery& param, const QByteArray& content);
    static ActionResult set_storage_mount(const QUrlQuery& param, const QByteArray& content);
    static ActionResult update4(const QUrlQuery& param, const QByteArray& content);
    static ActionResult restart(const QUrlQuery& param, const QByteArray& content);
    static ActionResult set_rec_start(const QUrlQuery& param, const QByteArray& content);
    static ActionResult set_rec_stop(const QUrlQuery& param, const QByteArray& content);
    static ActionResult get_insole_status(const QUrlQuery& param, const QByteArray& content);
    static ActionResult shutdown(const QUrlQuery &param, const QByteArray &content);
    static ActionResult set_time_ntp(const QUrlQuery &param, const QByteArray &content);

private:
    static bool time_lock;


    static QVector<doWorker*> workers;

    static ActionResult get_data2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult get_batt2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult active2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult version2(const QUrlQuery &param, const QByteArray &content);
    static ActionResult version4_2(const QUrlQuery &param, const QByteArray &content);
    static ActionResult version5_2(const QUrlQuery &param, const QByteArray &content);
    static ActionResult instance2(const QUrlQuery &param, const QByteArray &content);
    static ActionResult hwinfo2(const QUrlQuery &param, const QByteArray &content);
    static ActionResult get_data_length2(const QUrlQuery &param, const QByteArray &content);
    static ActionResult get_storage_status2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult set_storage_mount2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult update4_2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult restart2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult startrec2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult stoprec2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult mountstorage2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult getstoragestatus2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult get_insole_status2(const QUrlQuery& param, const QByteArray& content);
    static ActionResult set_time_ntp2(const QUrlQuery &param, const QByteArray &content);

    static ActionResult shutdown2(const QUrlQuery &param, const QByteArray &content);
    static const QString _cannot_start_rec;

    static const QString ERROR;
    static const QString STATUS;
    static const QString DATAPATH_KEY;
    static const ActionResult _ok;
    //static const ActionResult _locked;
    //static const ActionResult _notimplemented;
    //static const ActionResult _busy;
    static const ActionResult _not_mounted;
    static const ActionResult _wrong_ix;
    static const ActionResult _no_ix;
    static const ActionResult _uninitialised;
    static const ActionResult _no_data;
    static const ActionResult _no_file;
    //static const ActionResult _wrong_data;
    static const ActionResult _no_folder;
    static const ActionResult _name_too_long;
    static const ActionResult _wrong_name;
    static const ActionResult _data_too_long;
    static const ActionResult _not_set;

    struct Error{
        const ActionResult wrong_data =
            {ERROR+nameof(wrong_data),
             ActionResult::Status::Codes::BadRequest};

        const ActionResult notimplemented =
            {ERROR+nameof(notimplemented),
             ActionResult::Status::Codes::NotImplemented};
        const ActionResult busy =
            {ERROR+nameof(busy),
             ActionResult::Status::Codes::InternalServerError};
        const ActionResult locked =
            {ERROR+nameof(locked),
             ActionResult::Status::Codes::InternalServerError};
        const ActionResult not_opened =
            {ERROR+nameof(not_opened),
             ActionResult::Status::Codes::InternalServerError};
        const ActionResult not_inited =
            {ERROR+nameof(not_inited),
             ActionResult::Status::Codes::InternalServerError};
        const ActionResult recording =
            {ERROR+nameof(recording),
             ActionResult::Status::Codes::InternalServerError};

    };


    static const Error _error;

    static ActionResult work(const QUrlQuery& param, const QByteArray& content, ActionWorker, bool* lock, int timeout);
};

#endif // DOWORK_H
