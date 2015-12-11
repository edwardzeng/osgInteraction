#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgFX/Scribe>
#include <osgGA/GUIEventHandler>
#include <osgUtil/Optimizer>
#include <iostream>
//对象选取事件处理器
class PickHandler : public osgGA::GUIEventHandler
{
public:
	PickHandler():
	  _mx(0.0f),
		  _my(0.0f),
		  _lastSelectNode(0)
	  {
		  //
	  }
	  ~PickHandler()
	  {
		  //
	  }
	  //事件处理函数
	  bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
	  {
		  osg::ref_ptr<osgViewer::View> view = dynamic_cast<osgViewer::View*>(&aa);
		  if (!view) return false;
		  switch(ea.getEventType())
		  {
			  //鼠标按下
		  case(osgGA::GUIEventAdapter::PUSH):
			  {
				  //更新鼠标位置
				  _mx = ea.getX();
				  _my = ea.getY();
				  pick(view.get(), ea.getX(), ea.getY());
				  break;
			  }
		  case(osgGA::GUIEventAdapter::RELEASE):
			  {
				  if (_mx==ea.getX() && _my==ea.getY())
				  {
					  //执行对象选取
					  //pick(view.get(), ea.getX(), ea.getY());
				  }
				  break;
			  }
		  default:
			  break;
		  }
		  return false;
	  }
	  //对象选取事件处理器
	  void pick(osg::ref_ptr<osgViewer::View> view, float x, float y)
	  {
		  osg::ref_ptr<osg::Node> node;
		  osg::ref_ptr<osg::Node> node3;
		  osg::ref_ptr<osg::Group> parent;
		  //创建一个线段交集检测函数
		  osgUtil::LineSegmentIntersector::Intersections intersections;
		  if (view->computeIntersections(x, y, intersections))
		  {
			  osgUtil::LineSegmentIntersector::Intersections::iterator it = intersections.begin();
			  const osg::NodePath& nodePath = it->nodePath;

			  if (!it->nodePath.empty()&&!(it->nodePath.back()->getName().empty()))
			  {
				  //const osg::NodePath & nodePath= it->nodePath;
				  for (int i=0;i<nodePath.size();i++)
				  {
					  osg::Node* nd=dynamic_cast<osg::Node*>(nodePath[i]);
					  if (nd->getName()=="cow.osg")
					  {
						  std::cout<<"found arm1.ive"<<std::endl;
					  }
					  std::cout<<"node"<<i<<":"<<nd->getName()<<std::endl;
				  }
				  std::cout<<"-------------------- "<<std::endl<<std::endl;
			  }


			  //得到选择的物体
			  node = (nodePath.size()>=1)?nodePath[2]:0;
			  node3 = (nodePath.size()>=4)?nodePath[3]:0;
			  parent = (nodePath.size()>=2)?dynamic_cast<osg::Group*>(nodePath[1]):0;
		  }       
		  //用一种高亮显示来显示物体已经被选中
		  if (parent.get() && node.get())
		  {
			  bool isSelectSameNode=false;
			  if (_lastSelectNode)
			  {
				  if ((_lastSelectNode ==node) || (_lastSelectNode ==node3))
				  {
					  isSelectSameNode=true;
				  }
				  else
				  {
					  //如果选择其他物体，则移除高亮显示的对象
					  _lastSelectParent->replaceChild(_lastScribe,_lastSelectNode);
					  _lastSelectNode=0;
				  }
			  }
			  if (!isSelectSameNode)
			  {
				  //如果对象选择到，高亮显示
				  _lastSelectNode=node;
				  _lastSelectParent=parent;
				  osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe();
				  scribe->addChild(node.get());
				  scribe->setName("scribe node");
				  _lastScribe=scribe;
				  parent->replaceChild(node.get(),scribe.get());
			  }
		  }
	  }
public:
	//得到鼠标的位置
	float _mx ;
	float _my;
	osg::ref_ptr<osg::Node> _lastSelectNode;
	osg::ref_ptr<osg::Group> _lastSelectParent;
	osg::ref_ptr<osgFX::Scribe> _lastScribe;
};
int main()
{
	//创建Viewer对象，场景浏览器
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	viewer->addEventHandler(new PickHandler());
	//创建场景组节点
	osg::ref_ptr<osg::Group> root = new osg::Group();
	osg::ref_ptr<osg::MatrixTransform> transform1=new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> transform2=new osg::MatrixTransform;

	osg::ref_ptr<osg::Node> arm1 = osgDB::readNodeFile("E:/SourceCode/OSG/OSGData/Robot/right/arm1.ive");
	osg::ref_ptr<osg::Node> cow= osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/cow.osg");
	osg::ref_ptr<osg::Node> car= osgDB::readNodeFile("E:/SourceCode/OSG/OSGData/Robot/car.osg");

	arm1->setName("arm1.ive");
	cow->setName("cow.osg");
	car->setName("car.ive");
	viewer->getCamera()->setName("master camera");

	transform1->setName("tran1");
	transform2->setName("tran2");
	transform1->addChild(arm1);

	transform1->addChild(transform2);
	transform2->setMatrix(osg::Matrix::translate(0.,0.,2000.));
	transform2->addChild(car);
	root->setName("root group");
	//添加到场景
	//root->addChild(node);
	//root->addChild(cow);
	//root->addChild(car);
	root->addChild(transform1);
	//优化场景数据
	osgUtil::Optimizer optimizer ;
	optimizer.optimize(root.get()) ;
	viewer->setSceneData(root.get());
	viewer->setUpViewInWindow(300,50,800,600);
	viewer->realize();
	viewer->run();
	return 0 ;
}