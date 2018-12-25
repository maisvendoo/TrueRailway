#include    "vehicle-loader.h"

#include    "config-reader.h"
#include    "filesystem.h"

#include    <osgDB/FileUtils>
#include    <osgDB/FileNameUtils>
#include    <osgDB/ReadFile>
#include    <osg/Texture2D>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
osg::Texture2D *createTexture(const std::string &path)
{
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;

    std::string fileName = osgDB::findDataFile(path);

    if (fileName.empty())
        return nullptr;

    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(fileName);

    if (!image.valid())
        return nullptr;

    std::string ext = osgDB::getLowerCaseFileExtension(fileName);

    if (ext == "tga")
    {
        image->flipVertical();
    }

    texture->setImage(image.get());
    texture->setNumMipmapLevels(0);
    texture->setFilter(osg::Texture::MIN_FILTER , osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER , osg::Texture::LINEAR);
    texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
    texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
    texture->setUnRefImageDataAfterApply(true);

    return texture.release();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
osg::Group *loadVehicle(const std::string &configPath)
{
    FileSystem &fs = FileSystem::getInstance();
    std::string cfg_path = fs.combinePath(fs.getVehiclesDir(), configPath + fs.separator() + configPath + ".xml");
    ConfigReader cfg(cfg_path);

    std::string modelName = "";
    std::string textureName = "";
    std::string wheelModelName = "";
    std::string wheelTextureName = "";

    if (cfg.isOpenned())
    {
        std::string secName = "Vehicle";
        cfg.getValue(secName, "ExtModelName", modelName);
        cfg.getValue(secName, "ExtTextureName", textureName);
    }

    std::string modelPath = osgDB::findDataFile(fs.combinePath(fs.getVehicleModelsDir(), modelName));

    osg::ref_ptr<osg::Group> group = new osg::Group;

    osg::ref_ptr<osg::Node> model;

    if (!modelPath.empty())
    {
        modelPath = fs.toNativeSeparators(modelPath);
        model = osgDB::readNodeFile(modelPath);
        group->addChild(model.get());
    }

    if (!textureName.empty())
    {
        std::string tex_path = fs.combinePath(fs.getVehicleTexturesDir(), textureName);
        osg::ref_ptr<osg::Texture2D> texture = createTexture(tex_path);

        if (texture.valid())
        {
            model->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());

            std::string ext = osgDB::getLowerCaseFileExtension(tex_path);

            if (ext == "tga")
                model->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            else
                model->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
        }
    }

    return group.release();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
osg::MatrixTransform *loadWheels(const std::string &configPath)
{
    osg::ref_ptr<osg::MatrixTransform> rotate = new osg::MatrixTransform;

    FileSystem &fs = FileSystem::getInstance();
    std::string cfg_path = fs.combinePath(fs.getVehiclesDir(), configPath + fs.separator() + configPath + ".xml");
    ConfigReader cfg(cfg_path);

    std::string wheelModelName = "";
    std::string wheelTextureName = "";

    if (cfg.isOpenned())
    {
        std::string secName = "Vehicle";
        cfg.getValue(secName, "WheelModel", wheelModelName);
        cfg.getValue(secName, "WheelTexture", wheelTextureName);
    }

    std::string wheelModelPath = osgDB::findDataFile(fs.combinePath(fs.getVehicleModelsDir(), wheelModelName));

    osg::ref_ptr<osg::Node> model;

    if (!wheelModelName.empty())
    {
        wheelModelPath = fs.toNativeSeparators(wheelModelPath);
        model = osgDB::readNodeFile(wheelModelPath);
        rotate->addChild(model.get());
    }

    if (!wheelTextureName.empty())
    {
        std::string tex_path = fs.combinePath(fs.getVehicleTexturesDir(), wheelTextureName);
        osg::ref_ptr<osg::Texture2D> texture = createTexture(tex_path);

        if (texture.valid())
        {
            model->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());

            std::string ext = osgDB::getLowerCaseFileExtension(tex_path);

            if (ext == "tga")
                model->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            else
                model->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
        }
    }

    return rotate.release();
}
