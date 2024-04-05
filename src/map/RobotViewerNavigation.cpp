#include "RobotViewerNavigation.h"
#include <QTextCodec>

RobotViewerNavigation::RobotViewerNavigation()
    : bgl_topology_(nullptr)
    , is_run_(false)
    , cancel_(false)
    , is_loop_(true)
{
    // AGVROS.BindCallBackForNavigationFinish(
    //	std::bind(&RobotViewerNavigation::BindNavigationTaskFinish,this, _1));
    thread_ = std::shared_ptr<std::thread>(
        new std::thread(std::bind(&RobotViewerNavigation::MainLoop, this)));
}

RobotViewerNavigation::~RobotViewerNavigation()
{
    is_loop_ = false;
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
    thread_ = nullptr;
#if 0
    if(bgl_topology_) {
        delete bgl_topology_;
    }
#endif
    bgl_topology_ = nullptr;
}

bool RobotViewerNavigation::LoadFile(const QString &file_name)
{
    if (is_run_)
    {
        return false;
    }
    std::string strFile = file_name.toStdString();
    QTextCodec *code = QTextCodec::codecForLocale();
    if (code)
        strFile = code->fromUnicode(file_name).data();
#if 0
    return bgl_topology_->LoadTopoInfo_v4(strFile);
#else
    return true;
#endif
}

bool RobotViewerNavigation::GotoDest(const UPL &dest_upl)
{
#if 0
    CubicBezier bezier;
    UPL current_upl;
    {
        std::lock_guard<std::mutex> lock(upl_mutex_);
        current_upl= current_upl_;
    }
    
    //LOGS_INFO("RobotViewerNavigation")<<"";
    if (!bgl_topology_->GetEdge(std::pair<int,int>(dest_upl.start_id,dest_upl.end_id),bezier))
    {
        //找不到终点
        QVector<UPL> error_upl;
        error_upl.push_back(dest_upl);
        emit TaskError(error_upl,NaviagtionEndError);
        return false;
    }
    if (!bgl_topology_->GetEdge(std::pair<int,int>(current_upl.start_id,current_upl.end_id),bezier)
        &&!bgl_topology_->GetEdge(std::pair<int,int>(current_upl.end_id,current_upl.start_id),bezier))
    {
        //找不到起点
        QVector<UPL> error_upl;
        error_upl.push_back(current_upl);
        emit TaskError(error_upl,NaviagtionStartError);
        return false;
    }
    std::vector<UPL> result = bgl_topology_->GetListTopologyEdge(current_upl,dest_upl);
    if (result.empty())
    {
        //路径搜索失败
        QVector<UPL> error_upl;
        error_upl.push_back(current_upl);
        error_upl.push_back(dest_upl);
        emit TaskError(error_upl,NaviagtionPathError);
        return false;
    }
    dest_upl_ = dest_upl;
    if (ClientForNavigation(result))
    {
        QVector<UPL> upl_vec;
        for (size_t i = 0 ;i<result.size();++i)
        {
            upl_vec.push_back(result[i]);
        }
        emit TaskRoute(upl_vec);
        naviagtion_finish_ = false;
        return true;
    }
    QVector<UPL> error_upl;
    emit TaskError(error_upl,NaviagtionClose);
    return false;
#else
    return true;
#endif
}

bool RobotViewerNavigation::ClientForNavigation(const std::vector<UPL> &upls)
{
#if 0
    GNavigationTask task;
    double next_path = 999999;
    int last_target = 1;
    task.set_next_path_length(next_path);
    task.set_type(GNavigationTask_GNavigationType_STARTTASK);
    task.set_target_arrive(last_target);
    *task.mutable_target_upl()=UPL2GNavigationUPL(dest_upl_);
    for (unsigned int i=0;i<upls.size();++i)
    {
        GNavigationUPL * it_upl=task.add_target_edges();
        const UPL &first_upl = upls[i];
        if (i < upls.size()-1)
        {
            //如果有重复边 报错！
            const UPL &second_upl = upls[i+1];
            assert((first_upl.start_id!=second_upl.start_id)||(first_upl.end_id!=second_upl.end_id));
        }
        *it_upl=UPL2GNavigationUPL(first_upl);
    }
    return AGVROS.CallNavigationTask(task);
#else
    return true;
#endif
}

bool RobotViewerNavigation::ClientForNavigation(int status)
{
#if 0
    GNavigationTask task;
    task.set_type((GNavigationTask_GNavigationType)status);
    return AGVROS.CallNavigationTask(task);
#else
    return true;
#endif
}

#if 0
GNavigationUPL RobotViewerNavigation::UPL2GNavigationUPL( const UPL& upl )
{
    GNavigationUPL g_upl;
    g_upl.set_start_id(upl.start_id);
    g_upl.set_end_id(upl.end_id);
    g_upl.set_percent(upl.percent);
    g_upl.set_forward(upl.forward);
    return g_upl;
}
#endif

void RobotViewerNavigation::BindNavigationTaskFinish(bool finish)
{
    std::lock_guard<std::mutex> lock(mutex_);
    naviagtion_finish_ = true;
}

bool RobotViewerNavigation::StartTarget(const std::vector<UPL> &dests)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_run_)
    {
        return false;
    }
    is_run_ = true;
    dest_upls_ = dests;
    naviagtion_finish_ = true;
    return true;
}

void RobotViewerNavigation::MainLoop()
{
    try
    {
        while (is_loop_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::lock_guard<std::mutex> lock(mutex_);
            if (cancel_)
            {
                is_run_ = false;
                dest_upls_.clear();
                cancel_ = false;
                emit TaskFinish(false);
            }
            if (is_run_)
            {
                if (naviagtion_finish_)
                {
                    if (!dest_upls_.empty())
                    {
                        if (GotoDest(dest_upls_[0]))
                        {
                            dest_upls_.pop_back();
                        }
                        else
                        {
                            is_run_ = false;
                            dest_upls_.clear();
                            emit TaskFinish(false);
                        }
                    }
                    else
                    {
                        is_run_ = false;
                        emit TaskFinish(true);
                    }
                }
            }
        }
    }
    catch (...)
    {
        return;
    }
}

bool RobotViewerNavigation::CancelTarget()
{
    std::lock_guard<std::mutex> lock(mutex_);
    // ClientForNavigation(GNavigationTask_GNavigationType_CANCELTAST);
    cancel_ = true;
    return true;
}

bool RobotViewerNavigation::PauseTarget()
{
    std::lock_guard<std::mutex> lock(mutex_);
#if 0
    return ClientForNavigation(GNavigationTask_GNavigationType_PAUSE);
#else
    return true;
#endif
}

bool RobotViewerNavigation::ResumeTarget()
{
    std::lock_guard<std::mutex> lock(mutex_);
#ifdef ENABLE_TEST
    // return ClientForNavigation(GNavigationTask_GNavigationType_RESUME);
#else
    return true;
#endif
}

void RobotViewerNavigation::SetCurrentUpl(const UPL &current_upl)
{
    std::lock_guard<std::mutex> lock(upl_mutex_);
    current_upl_ = current_upl;
}
