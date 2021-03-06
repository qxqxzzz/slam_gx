/* ****************************************
 * SLAM end: 基于图优化的SLAM端，
 * 从Graphic end中读取数据并估计机器人位置与路标

 * 有关id的规定：
 * 由于在图优化中，机器人位置与路标都算成单独结点，所以我们要为它们独立计算ID
 * 因此定义了ROBOT_START_ID和LANDMARK_START_ID两个常量
 * 但这也会限制图的总大小
 
 ****************************************/
#pragma once

#include "GraphicEnd.h"
#include "const.h"
#include <g2o/types/slam2d/vertex_se2.h>
#include <g2o/types/slam2d/vertex_point_xy.h>
#include <g2o/types/slam2d/edge_se2.h>
#include <g2o/types/slam2d/edge_se2_pointxy.h>
#include <g2o/types/slam2d/types_slam2d.h>

#include <g2o/core/sparse_optimizer.h>
#include <g2o/core/block_solver.h>
#include <g2o/core/factory.h>
#include <g2o/core/optimization_algorithm_factory.h>
#include <g2o/core/optimization_algorithm_gauss_newton.h>
#include <g2o/solvers/csparse/linear_solver_csparse.h>
#include <g2o/core/robust_kernel.h>
#include <g2o/core/robust_kernel_factory.h>


#include <sstream>
using namespace std;
using namespace g2o;

typedef BlockSolver< BlockSolverTraits<-1, -1> >  SlamBlockSolver;
typedef LinearSolverCSparse<SlamBlockSolver::PoseMatrixType> SlamLinearSolver;


class SLAMEnd
{
 public:
    SLAMEnd(GraphicEnd* pGraphicEnd);
    ~SLAMEnd();

 public:
    int optimize();
    int optimize_once();

    //求解一次优化问题
    void solve()
    {
        //固定第一个状态点
        VertexSE2* firstRobotPose = dynamic_cast<VertexSE2*>(optimizer.vertex(0));
        firstRobotPose->setFixed(true);
        optimizer.setVerbose(true);

        optimizer.initializeOptimization();
        optimizer.optimize(_optimize_step);
    }

    //重要：将优化结果反馈至图形端
    void feedback();
    void save()
    {
        stringstream ss;
        ss<<"g2o/"<<_pGraphicEnd->_loops<<".g2o";
        string str;
        ss>>str;
        optimizer.save(str.c_str());  
    }
    void testOptimization(string fileAddr);
 private:
    //私有函数，内部调用
    void AddRobotPose();  //增加一个机器人位置结点
    void AddLandmark();   //增加路标结点以及对应的边
    
 protected:
    SparseOptimizer optimizer;
    GraphicEnd* _pGraphicEnd;
    FeatureManager* _pFeatureManager;
    FeatureGrabberBase* _pFeatureGrabber;
    ImageReaderBase* _pImageReader;

    int _optimize_step;
    SE2 _prev;            //上一次循环的机器人位置
    RobustKernel* _robust_kernel_ptr;

 public:
    //公开数据成员
    int _robot_id;
    int _landmark_id;
};
