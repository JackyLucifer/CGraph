/***************************
@Author: Chunel
@Contact: chunel@foxmail.com
@File: GElement.cpp
@Time: 2021/6/1 10:13 下午
@Desc: 
***************************/

#include "GElement.h"

CGRAPH_NAMESPACE_BEGIN

const std::string& GElement::getName() const {
    return this->name_;
}


const std::string& GElement::getSession() const {
    return this->session_;
}


GElement::GElement() {
    this->session_ = CGRAPH_GENERATE_SESSION
}


GElement::~GElement() {
    CGRAPH_DELETE_PTR(aspect_manager_)
}


CStatus GElement::beforeRun() {
    CGRAPH_FUNCTION_BEGIN
    this->done_ = false;
    this->left_depend_ = dependence_.size();

    CGRAPH_FUNCTION_END
}


CStatus GElement::afterRun() {
    CGRAPH_FUNCTION_BEGIN

    for (auto& element : this->run_before_) {
        element->left_depend_--;
    }
    this->done_ = true;

    CGRAPH_FUNCTION_END
}


GElementPtr GElement::setName(const std::string& name) {
    CGRAPH_ASSERT_INIT_RETURN_NULL(false)
    this->name_ = name.empty() ? this->session_ : name;

    // 设置name信息的时候，顺便给 aspect_manager_ 一起设置了
    if (aspect_manager_) {
        aspect_manager_->setName(name);
    }
    return this;
}


GElement* GElement::setLoop(CSize loop) {
    CGRAPH_ASSERT_INIT_RETURN_NULL(false)

    this->loop_ = loop;
    return this;
}


GElement* GElement::setLevel(CLevel level) {
    CGRAPH_ASSERT_INIT_RETURN_NULL(false)

    this->level_ = level;
    return this;
}


CBool GElement::isRunnable() const {
    return 0 >= this->left_depend_ && !this->done_;
}


CBool GElement::isLinkable() const {
    return this->linkable_;
}


CStatus GElement::process(bool isMock) {
    CGRAPH_NO_SUPPORT
}


CStatus GElement::addDependGElements(const GElementPtrSet& elements) {
    CGRAPH_FUNCTION_BEGIN
    for (GElementPtr cur: elements) {
        if (this == cur) {
            continue;
        }

        cur->run_before_.insert(this);
        this->dependence_.insert(cur);
    }

    this->left_depend_ = this->dependence_.size();
    CGRAPH_FUNCTION_END
}


CStatus GElement::setElementInfo(const GElementPtrSet& dependElements,
                                 const std::string& name,
                                 CSize loop,
                                 CLevel level,
                                 GParamManagerPtr paramManager,
                                 UThreadPoolPtr threadPool) {
    CGRAPH_FUNCTION_BEGIN
    CGRAPH_ASSERT_NOT_NULL(threadPool)
    CGRAPH_ASSERT_INIT(false)

    this->setName(name)->setLoop(loop)->setLevel(level);
    param_manager_ = paramManager;
    thread_pool_ = threadPool;
    status = this->addDependGElements(dependElements);
    CGRAPH_FUNCTION_END
}


CStatus GElement::doAspect(const GAspectType& aspectType, const CStatus& curStatus) {
    CGRAPH_FUNCTION_BEGIN

    // 如果切面管理类为空，或者未添加切面，直接返回
    if (this->aspect_manager_
        && 0 != this->aspect_manager_->getSize()) {
        status = aspect_manager_->reflect(aspectType, curStatus);
    }

    CGRAPH_FUNCTION_END
}


CStatus GElement::fatProcessor(const CFunctionType& type) {
    CGRAPH_FUNCTION_BEGIN

    try {
        switch (type) {
            case CFunctionType::RUN: {
                for (CSize i = 0; i < this->loop_; i++) {
                    /** 执行带切面的run方法 */
                    status = doAspect(GAspectType::BEGIN_RUN);
                    CGRAPH_FUNCTION_CHECK_STATUS
                    do {
                        status = run();
                        /**
                         * 如果状态是ok的，并且被条件hold住，则循环执行
                         * 默认所有element的isHold条件均为false，即不hold，即执行一次
                         * 可以根据需求，对任意element类型，添加特定的isHold条件
                         * */
                    } while (status.isOK() && this->isHold());
                    doAspect(GAspectType::FINISH_RUN, status);
                }
                break;
            }
            case CFunctionType::INIT: {
                status = doAspect(GAspectType::BEGIN_INIT);
                CGRAPH_FUNCTION_CHECK_STATUS
                status = init();
                doAspect(GAspectType::FINISH_INIT, status);
                break;
            }
            case CFunctionType::DESTROY: {
                status = doAspect(GAspectType::BEGIN_DESTROY);
                CGRAPH_FUNCTION_CHECK_STATUS
                status = destroy();
                doAspect(GAspectType::FINISH_DESTROY, status);
                break;
            }
            default:
                CGRAPH_RETURN_ERROR_STATUS("get function type error")
        }
    } catch (const CException& ex) {
        status = crashed(ex);
    }

    CGRAPH_FUNCTION_END
}


CBool GElement::isHold() {
    /**
     * 默认仅返回false
     * 可以根据自己逻辑，来实现"持续循环执行，直到特定条件出现的时候停止"的逻辑
     */
    return false;
}


CStatus GElement::crashed(const CException& ex) {
    /**
     * 默认直接抛出异常的
     * 如果需要处理的话，可以通过覆写此函数来
     */
    CGRAPH_THROW_EXCEPTION(ex.what())
}

CGRAPH_NAMESPACE_END
