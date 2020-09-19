#include "DebugHandler.h"

#include <bitset>
#include <iterator>

#include <osgViewer/Viewer>
#include <osg/Program>
#include <OsgQuery.h>

namespace toy
{

std::string to_string(osg::StateSet::RenderBinMode mode)
{
    switch (mode)
    {
        case osg::StateSet::INHERIT_RENDERBIN_DETAILS:
            return "INHERIT_RENDERBIN_DETAILS";
        case osg::StateSet::OVERRIDE_PROTECTED_RENDERBIN_DETAILS:
            return "OVERRIDE_PROTECTED_RENDERBIN_DETAILS";
        case osg::StateSet::OVERRIDE_RENDERBIN_DETAILS:
            return "OVERRIDE_RENDERBIN_DETAILS";
        case osg::StateSet::PROTECTED_RENDERBIN_DETAILS:
            return "PROTECTED_RENDERBIN_DETAILS";
        case osg::StateSet::USE_RENDERBIN_DETAILS:
            return "USE_RENDERBIN_DETAILS";
        default:
            return "";
    }
}

std::string to_string(osgUtil::RenderBin::SortMode mode)
{
    switch (mode)
    {
        case osgUtil::RenderBin::SORT_BACK_TO_FRONT:
            return "SORT_BACK_TO_FRONT";
        case osgUtil::RenderBin::SORT_BY_STATE:
            return "SORT_BY_STATE";
        case osgUtil::RenderBin::SORT_BY_STATE_THEN_FRONT_TO_BACK:
            return "SORT_BY_STATE_THEN_FRONT_TO_BACK";
        case osgUtil::RenderBin::SORT_FRONT_TO_BACK:
            return "SORT_FRONT_TO_BACK";
        case osgUtil::RenderBin::TRAVERSAL_ORDER:
            return "TRAVERSAL_ORDER";
        default:
            return "";
    }
}

#ifdef PRINT_STAGE
RenderStagePrinter::RenderStagePrinter(std::ostream& out) : _out(out) {}

void RenderStagePrinter::drawImplementation(
    osgUtil::RenderBin* bin, osg::RenderInfo& renderInfo, osgUtil::RenderLeaf*& previous)
{
    if (_enabled)
    {
        auto stage = dynamic_cast<osgUtil::RenderStage*>(bin);
        _renderInfo = &renderInfo;
        if (stage)
        {
            _out << std::string(60, '-') << "\n";
            _out << "Frame " << renderInfo.getState()->getFrameStamp()->getFrameNumber()
                 << "\n";
            pushRenderStage(0, 0, stage);
            printRenderStage(stage);
            popRenderStage();
        }
        _enabled = false;
    }

    bin->drawImplementation(renderInfo, previous);
}

void RenderStagePrinter::printRenderStage(const osgUtil::RenderStage* stage)
{
    auto& preStages = stage->getPreRenderList();
    for (auto& item: preStages)
    {
        pushRenderStage(-1, item.first, item.second);
        printRenderStage(item.second);
        popRenderStage();
    }

    std::cout << std::string(40, '=') << std::endl;
    pushRenderBin(stage);
    printRenderBin(stage);
    popRenderBin();

    auto& postStages = stage->getPostRenderList();
    for (auto& item: postStages)
    {
        pushRenderStage(1, item.first, item.second);
        printRenderStage(item.second);
        popRenderStage();
    }
}

void RenderStagePrinter::pushRenderStage(
    int renderOrder, int order, const osgUtil::RenderStage* stage)
{
    _stages.push_back({renderOrder, order, stage});
}

void RenderStagePrinter::popRenderStage()
{
    _stages.pop_back();
}

void RenderStagePrinter::pushRenderBin(const osgUtil::RenderBin* bin)
{
    _bins.push_back(bin);
}

void RenderStagePrinter::popRenderBin()
{
    _bins.pop_back();
}

void RenderStagePrinter::printRenderBin(const osgUtil::RenderBin* bin)
{
    // print child bins with binNum <0
    auto& bins = bin->getRenderBinList();
    for (auto it = bins.begin(); it->first < 0 && it != bins.end(); ++it)
    {
        pushRenderBin(it->second);
        printRenderBin(it->second);
        popRenderBin();
    }

    // print stage path, bin path
    std::cout << std::string(20, '*') << std::endl;
    printPath();

    auto& fineGrained = bin->getRenderLeafList();
    if (!fineGrained.empty())
    {
        printStateset(bin->getStateSet());
        _out << "--------\n";
        _out << "fine grained :\n";

        const osgUtil::StateGraph* previousStateGrouph = 0;
        const osg::StateSet* previousStateSet = 0;

        for (auto leaf: fineGrained)
        {
            if (previousStateGrouph != leaf->_parent)
            {
                // fine grained renderleaf is sorted back to front, it's not grouped by
                // StateGraph.
                _out << "StateGraph : " << leaf->_parent << "\n";
                auto ss = leaf->_parent->getStateSet();
                if (ss != previousStateSet)
                {
                    printStateset(ss);
                }
                previousStateGrouph = leaf->_parent;
                previousStateSet = ss;
            }
            _out << "depth : " << leaf->_depth << "\n";
            printLeaf(leaf);
        }
    }

    if (!bin->getStateGraphList().empty())
    {
        _out << "coarse grained :\n";
        for (auto graph: bin->getStateGraphList())
        {
            _out << "StateGraph : " << graph << "\n";
            printStateset(graph->getStateSet());
            for (auto leaf: graph->_leaves)
            {
                printLeaf(leaf);
            }
        }
    }

    // print child bins with binNum >0
    for (auto it = bins.begin(); it->first > 0 && it != bins.end(); ++it)
    {
        pushRenderBin(it->second);
        printRenderBin(it->second);
        popRenderBin();
    }
}

void RenderStagePrinter::printPath()
{
    for (auto i = 0; i < _stages.size(); ++i)
    {
        auto& stageNode = _stages[i];
        if (stageNode.renderOrder == 0)
        {
            _out << ":" << stageNode.stage;
        }
        else
        {
            _out << (stageNode.renderOrder < 0 ? "<--" : "-->");
            _out << stageNode.order << "-" << stageNode.stage;
        }

        auto camera = stageNode.stage->getCamera();
        if (camera && !camera->getName().empty())
        {
            _out << "(camera : " << camera->getName() << ")";
        }
        _out << "\n";
    }
    _out << "\n";
    std::string binIndent = "    ";

    for (auto i = 0; i < _bins.size(); ++i)
    {
        auto bin = _bins[i];
        _out << binIndent << bin->getBinNum() << "-" << bin << "("
             << to_string(bin->getSortMode()) << ")";

        if (bin->getStateSet())
        {
            _out << ", StateSet : " << bin->getStateSet();
        }
        _out << "\n";
    }
    _out << "\n";
}

std::string stateAttributeTypeToString(osg::StateAttribute::Type type)
{
    switch (type)
    {
        case osg::StateAttribute::TEXTURE:
            return "TEXTURE";
        case osg::StateAttribute::POLYGONMODE:
            return "POLYGONMODE";
        case osg::StateAttribute::POLYGONOFFSET:
            return "POLYGONOFFSET";
        case osg::StateAttribute::MATERIAL:
            return "MATERIAL";
        case osg::StateAttribute::ALPHAFUNC:
            return "ALPHAFUNC";
        case osg::StateAttribute::ANTIALIAS:
            return "ANTIALIAS";
        case osg::StateAttribute::COLORTABLE:
            return "COLORTABLE";
        case osg::StateAttribute::CULLFACE:
            return "CULLFACE";
        case osg::StateAttribute::FOG:
            return "FOG";
        case osg::StateAttribute::FRONTFACE:
            return "FRONTFACE";
        case osg::StateAttribute::LIGHT:
            return "LIGHT";
        case osg::StateAttribute::POINT:
            return "POINT";
        case osg::StateAttribute::LINEWIDTH:
            return "LINEWIDTH";
        case osg::StateAttribute::LINESTIPPLE:
            return "LINESTIPPLE";
        case osg::StateAttribute::POLYGONSTIPPLE:
            return "POLYGONSTIPPLE";
        case osg::StateAttribute::SHADEMODEL:
            return "SHADEMODEL";
        case osg::StateAttribute::TEXENV:
            return "TEXENV";
        case osg::StateAttribute::TEXENVFILTER:
            return "TEXENVFILTER";
        case osg::StateAttribute::TEXGEN:
            return "TEXGEN";
        case osg::StateAttribute::TEXMAT:
            return "TEXMAT";
        case osg::StateAttribute::LIGHTMODEL:
            return "LIGHTMODEL";
        case osg::StateAttribute::BLENDFUNC:
            return "BLENDFUNC";
        case osg::StateAttribute::BLENDEQUATION:
            return "BLENDEQUATION";
        case osg::StateAttribute::LOGICOP:
            return "LOGICOP";
        case osg::StateAttribute::STENCIL:
            return "STENCIL";
        case osg::StateAttribute::COLORMASK:
            return "COLORMASK";
        case osg::StateAttribute::DEPTH:
            return "DEPTH";
        case osg::StateAttribute::VIEWPORT:
            return "VIEWPORT";
        case osg::StateAttribute::SCISSOR:
            return "SCISSOR";
        case osg::StateAttribute::BLENDCOLOR:
            return "BLENDCOLOR";
        case osg::StateAttribute::MULTISAMPLE:
            return "MULTISAMPLE";
        case osg::StateAttribute::CLIPPLANE:
            return "CLIPPLANE";
        case osg::StateAttribute::COLORMATRIX:
            return "COLORMATRIX";
        case osg::StateAttribute::VERTEXPROGRAM:
            return "VERTEXPROGRAM";
        case osg::StateAttribute::FRAGMENTPROGRAM:
            return "FRAGMENTPROGRAM";
        case osg::StateAttribute::POINTSPRITE:
            return "POINTSPRITE";
        case osg::StateAttribute::PROGRAM:
            return "PROGRAM";
        case osg::StateAttribute::CLAMPCOLOR:
            return "CLAMPCOLOR";
        case osg::StateAttribute::HINT:
            return "HINT";
        case osg::StateAttribute::SAMPLEMASKI:
            return "SAMPLEMASKI";
        case osg::StateAttribute::PRIMITIVERESTARTINDEX:
            return "PRIMITIVERESTARTINDEX";
        case osg::StateAttribute::CLIPCONTROL:
            return "CLIPCONTROL";
        case osg::StateAttribute::VALIDATOR:
            return "VALIDATOR";
        case osg::StateAttribute::VIEWMATRIXEXTRACTOR:
            return "VIEWMATRIXEXTRACTOR";
        case osg::StateAttribute::OSGNV_PARAMETER_BLOCK:
            return "OSGNV_PARAMETER_BLOCK";
        case osg::StateAttribute::OSGNVEXT_TEXTURE_SHADER:
            return "OSGNVEXT_TEXTURE_SHADER";
        case osg::StateAttribute::OSGNVEXT_VERTEX_PROGRAM:
            return "OSGNVEXT_VERTEX_PROGRAM";
        case osg::StateAttribute::OSGNVEXT_REGISTER_COMBINERS:
            return "OSGNVEXT_REGISTER_COMBINERS";
        case osg::StateAttribute::OSGNVCG_PROGRAM:
            return "OSGNVCG_PROGRAM";
        case osg::StateAttribute::OSGNVSLANG_PROGRAM:
            return "OSGNVSLANG_PROGRAM";
        case osg::StateAttribute::OSGNVPARSE_PROGRAM_PARSER:
            return "OSGNVPARSE_PROGRAM_PARSER";
        case osg::StateAttribute::UNIFORMBUFFERBINDING:
            return "UNIFORMBUFFERBINDING";
        case osg::StateAttribute::TRANSFORMFEEDBACKBUFFERBINDING:
            return "TRANSFORMFEEDBACKBUFFERBINDING";
        case osg::StateAttribute::ATOMICCOUNTERBUFFERBINDING:
            return "ATOMICCOUNTERBUFFERBINDING";
        case osg::StateAttribute::PATCH_PARAMETER:
            return "PATCH_PARAMETER";
        case osg::StateAttribute::FRAME_BUFFER_OBJECT:
            return "FRAME_BUFFER_OBJECT";
        case osg::StateAttribute::VERTEX_ATTRIB_DIVISOR:
            return "VERTEX_ATTRIB_DIVISOR";
        case osg::StateAttribute::SHADERSTORAGEBUFFERBINDING:
            return "SHADERSTORAGEBUFFERBINDING";
        case osg::StateAttribute::INDIRECTDRAWBUFFERBINDING:
            return "INDIRECTDRAWBUFFERBINDING";
        case osg::StateAttribute::VIEWPORTINDEXED:
            return "VIEWPORTINDEXED";
        case osg::StateAttribute::DEPTHRANGEINDEXED:
            return "DEPTHRANGEINDEXED";
        case osg::StateAttribute::SCISSORINDEXED:
            return "SCISSORINDEXED";
        case osg::StateAttribute::BINDIMAGETEXTURE:
            return "BINDIMAGETEXTURE";
        case osg::StateAttribute::SAMPLER:
            return "SAMPLER";
        default:
            throw std::runtime_error(
                "illegal state attribute type : " + std::to_string(type));
    }
}

osg::StateAttribute::Type stringToStateAttributeType(const std::string& s)
{
    if (s == "TEXTURE")
        return osg::StateAttribute::TEXTURE;
    if (s == "POLYGONMODE")
        return osg::StateAttribute::POLYGONMODE;
    if (s == "POLYGONOFFSET")
        return osg::StateAttribute::POLYGONOFFSET;
    if (s == "MATERIAL")
        return osg::StateAttribute::MATERIAL;
    if (s == "ALPHAFUNC")
        return osg::StateAttribute::ALPHAFUNC;
    if (s == "ANTIALIAS")
        return osg::StateAttribute::ANTIALIAS;
    if (s == "COLORTABLE")
        return osg::StateAttribute::COLORTABLE;
    if (s == "CULLFACE")
        return osg::StateAttribute::CULLFACE;
    if (s == "FOG")
        return osg::StateAttribute::FOG;
    if (s == "FRONTFACE")
        return osg::StateAttribute::FRONTFACE;
    if (s == "LIGHT")
        return osg::StateAttribute::LIGHT;
    if (s == "POINT")
        return osg::StateAttribute::POINT;
    if (s == "LINEWIDTH")
        return osg::StateAttribute::LINEWIDTH;
    if (s == "LINESTIPPLE")
        return osg::StateAttribute::LINESTIPPLE;
    if (s == "POLYGONSTIPPLE")
        return osg::StateAttribute::POLYGONSTIPPLE;
    if (s == "SHADEMODEL")
        return osg::StateAttribute::SHADEMODEL;
    if (s == "TEXENV")
        return osg::StateAttribute::TEXENV;
    if (s == "TEXENVFILTER")
        return osg::StateAttribute::TEXENVFILTER;
    if (s == "TEXGEN")
        return osg::StateAttribute::TEXGEN;
    if (s == "TEXMAT")
        return osg::StateAttribute::TEXMAT;
    if (s == "LIGHTMODEL")
        return osg::StateAttribute::LIGHTMODEL;
    if (s == "BLENDFUNC")
        return osg::StateAttribute::BLENDFUNC;
    if (s == "BLENDEQUATION")
        return osg::StateAttribute::BLENDEQUATION;
    if (s == "LOGICOP")
        return osg::StateAttribute::LOGICOP;
    if (s == "STENCIL")
        return osg::StateAttribute::STENCIL;
    if (s == "COLORMASK")
        return osg::StateAttribute::COLORMASK;
    if (s == "DEPTH")
        return osg::StateAttribute::DEPTH;
    if (s == "VIEWPORT")
        return osg::StateAttribute::VIEWPORT;
    if (s == "SCISSOR")
        return osg::StateAttribute::SCISSOR;
    if (s == "BLENDCOLOR")
        return osg::StateAttribute::BLENDCOLOR;
    if (s == "MULTISAMPLE")
        return osg::StateAttribute::MULTISAMPLE;
    if (s == "CLIPPLANE")
        return osg::StateAttribute::CLIPPLANE;
    if (s == "COLORMATRIX")
        return osg::StateAttribute::COLORMATRIX;
    if (s == "VERTEXPROGRAM")
        return osg::StateAttribute::VERTEXPROGRAM;
    if (s == "FRAGMENTPROGRAM")
        return osg::StateAttribute::FRAGMENTPROGRAM;
    if (s == "POINTSPRITE")
        return osg::StateAttribute::POINTSPRITE;
    if (s == "PROGRAM")
        return osg::StateAttribute::PROGRAM;
    if (s == "CLAMPCOLOR")
        return osg::StateAttribute::CLAMPCOLOR;
    if (s == "HINT")
        return osg::StateAttribute::HINT;
    if (s == "SAMPLEMASKI")
        return osg::StateAttribute::SAMPLEMASKI;
    if (s == "PRIMITIVERESTARTINDEX")
        return osg::StateAttribute::PRIMITIVERESTARTINDEX;
    if (s == "CLIPCONTROL")
        return osg::StateAttribute::CLIPCONTROL;
    if (s == "VALIDATOR")
        return osg::StateAttribute::VALIDATOR;
    if (s == "VIEWMATRIXEXTRACTOR")
        return osg::StateAttribute::VIEWMATRIXEXTRACTOR;
    if (s == "OSGNV_PARAMETER_BLOCK")
        return osg::StateAttribute::OSGNV_PARAMETER_BLOCK;
    if (s == "OSGNVEXT_TEXTURE_SHADER")
        return osg::StateAttribute::OSGNVEXT_TEXTURE_SHADER;
    if (s == "OSGNVEXT_VERTEX_PROGRAM")
        return osg::StateAttribute::OSGNVEXT_VERTEX_PROGRAM;
    if (s == "OSGNVEXT_REGISTER_COMBINERS")
        return osg::StateAttribute::OSGNVEXT_REGISTER_COMBINERS;
    if (s == "OSGNVCG_PROGRAM")
        return osg::StateAttribute::OSGNVCG_PROGRAM;
    if (s == "OSGNVSLANG_PROGRAM")
        return osg::StateAttribute::OSGNVSLANG_PROGRAM;
    if (s == "OSGNVPARSE_PROGRAM_PARSER")
        return osg::StateAttribute::OSGNVPARSE_PROGRAM_PARSER;
    if (s == "UNIFORMBUFFERBINDING")
        return osg::StateAttribute::UNIFORMBUFFERBINDING;
    if (s == "TRANSFORMFEEDBACKBUFFERBINDING")
        return osg::StateAttribute::TRANSFORMFEEDBACKBUFFERBINDING;
    if (s == "ATOMICCOUNTERBUFFERBINDING")
        return osg::StateAttribute::ATOMICCOUNTERBUFFERBINDING;
    if (s == "PATCH_PARAMETER")
        return osg::StateAttribute::PATCH_PARAMETER;
    if (s == "FRAME_BUFFER_OBJECT")
        return osg::StateAttribute::FRAME_BUFFER_OBJECT;
    if (s == "VERTEX_ATTRIB_DIVISOR")
        return osg::StateAttribute::VERTEX_ATTRIB_DIVISOR;
    if (s == "SHADERSTORAGEBUFFERBINDING")
        return osg::StateAttribute::SHADERSTORAGEBUFFERBINDING;
    if (s == "INDIRECTDRAWBUFFERBINDING")
        return osg::StateAttribute::INDIRECTDRAWBUFFERBINDING;
    if (s == "VIEWPORTINDEXED")
        return osg::StateAttribute::VIEWPORTINDEXED;
    if (s == "DEPTHRANGEINDEXED")
        return osg::StateAttribute::DEPTHRANGEINDEXED;
    if (s == "SCISSORINDEXED")
        return osg::StateAttribute::SCISSORINDEXED;
    if (s == "BINDIMAGETEXTURE")
        return osg::StateAttribute::BINDIMAGETEXTURE;
    if (s == "SAMPLER")
        return osg::StateAttribute::SAMPLER;
    else
        throw std::runtime_error("Unknown StateAttribute type string : " + s);
}

void RenderStagePrinter::printStateset(const osg::StateSet* ss)
{
    if (!ss)
    {
        return;
    }

    _out << "StateSet " << ss << "\n";

    auto& al = ss->getAttributeList();
    auto i = 0;
    for (auto iter = al.begin(); iter != al.end(); ++iter)
    {
        auto type = iter->first.first;
        auto member = iter->first.second;
        auto overrideValue = iter->second.second;
        _out << stateAttributeTypeToString(type) << "[" << member << "] " << std::hex
             << overrideValue << std::dec;

        if (type == osg::StateAttribute::PROGRAM)
        {
            auto prg = static_cast<const osg::Program*>(iter->second.first.get());
            // this will change State define str, it should not matter. This is not
            // reliable, you won't see the same handle in apitrace?
            auto pcp = prg->getPCP(const_cast<osg::State&>(*_renderInfo->getState()));
            _out << " {" << pcp->getHandle() << "}" << std::endl;
        }

        if (++i % 4 == 0 || iter == --al.end())
        {
            _out << "\n";
        }
        else
        {
            _out << " | ";
        }
    }
}

void RenderStagePrinter::printLeaf(const osgUtil::RenderLeaf* leaf)
{
    auto drawable = leaf->getDrawable();
    _out << "    " << drawable << " \"" << drawable->getName() << "\" "
         << (leaf->_dynamic ? "DYNAMIC" : "STATIC");

    auto& state = *_renderInfo->getState();

    // display list
#    ifdef OSG_GL_DISPLAYLISTS_AVAILABLE
    if (drawable->getUseDisplayList() &&
        !(state.useVertexBufferObject(drawable->getUseVertexBufferObjects())))
    {
        _out << " dl{" << drawable->getDisplayList(_renderInfo->getContextID()) << "}";
    }
#    endif /* ifndef  */

    // vao
    if (state.useVertexArrayObject(drawable->getUseVertexArrayObject()))
    {
        _out << " vao{"
             << drawable->getVertexArrayStateList()[_renderInfo->getContextID()]
                    ->getVertexArrayObject()
             << "}";
    }

    // vbo
    if (state.useVertexBufferObject(drawable->getUseVertexBufferObjects()))
    {
        auto geom = dynamic_cast<const osg::Geometry*>(drawable);
        if (geom)
        {
            // might create vertex buffer object, should not matter
            auto vbo = const_cast<osg::Geometry*>(geom)->getOrCreateVertexBufferObject();
            _out << " vbo{" << vbo->getGLBufferObject(_renderInfo->getContextID()) << "}"
                 << std::endl;
        }
    }
    _out << "\n";
}
#endif

namespace
{

std::ostream& printByte(std::ostream& os, unsigned char byte)
{
    if (byte == 0xff)
    {
        os << "ff";
    }
    else
    {
        os << std::bitset<8>(byte);
    }
    return os;
}

}  // namespace

void VerbosePrintVisitor::apply(osg::Node& node)
{
    output() << node.libraryName() << "::" << node.className() << " ( " << &node << " ) "
             << "\"" << node.getName() << "\" ";
    auto mask = node.getNodeMask();
    printByte(_out, mask >> 24 & 0xff) << " ";
    printByte(_out, mask >> 16 & 0xff) << " ";
    printByte(_out, mask >> 8 & 0xff) << " ";
    printByte(_out, mask >> 0 & 0xff);

    auto ss = node.getStateSet();
    if (ss)
    {
        _out << " StateSet( " << ss;
        if (ss->getRenderBinMode() != osg::StateSet::INHERIT_RENDERBIN_DETAILS)
        {
            _out << " " << to_string(ss->getRenderBinMode()) << " " << ss->getBinNumber()
                 << " \"" << ss->getBinName() << "\"";
        }

        _out << " )";
    }
    _out << "\n";
    enter();
    traverse(node);
    leave();
}

DebugHandler::DebugHandler(osg::Camera* camera)
{
#ifdef PRINT_STAGE
    _renderStagePrinter = new RenderStagePrinter(std::cout);
#endif
    setCamera(camera);
}

bool DebugHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    auto view = aa.asView();
    switch (ea.getEventType())
    {
        case osgGA::GUIEventAdapter::KEYDOWN:
            switch (ea.getKey())
            {
                case osgGA::GUIEventAdapter::KEY_F2:
                {
                    OSG_NOTICE << std::string(60, '-') << "\n";
                    VerbosePrintVisitor visitor(std::cout);
                    _camera->accept(visitor);
                    break;
                }
#ifdef PRINT_STAGE
                case osgGA::GUIEventAdapter::KEY_F3:
                    _renderStagePrinter->setEnabled(true);
                    break;
#endif

                case osgGA::GUIEventAdapter::KEY_F4:
                    osgDB::writeNodeFile(*_camera, "main.osgt",
                        new osgDB::Options("WriteImageHint=UseExternal"));
                    break;

                default:
                    break;
            }
            break;
        default:
            break;
    }
    return false;  // return true will stop event
}

void DebugHandler::setCamera(osg::Camera* v)
{
    _camera = v;
#ifdef PRINT_STAGE
    auto renderer = dynamic_cast<osgViewer::Renderer*>(_camera->getRenderer());
    if (renderer)
    {
        renderer->getSceneView(0)->getRenderStage()->setDrawCallback(_renderStagePrinter);
        renderer->getSceneView(1)->getRenderStage()->setDrawCallback(_renderStagePrinter);
    }

    // TODO slave cameras?
#endif
}

}  // namespace toy
