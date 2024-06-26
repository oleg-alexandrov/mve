#include <iostream>

#include "scenemanager.h"
#include "scene_addins/addin_sfm_renderer.h"

AddinSfmRenderer::AddinSfmRenderer (void)
{
    this->first_create_renderer = true;
    this->render_cb = new QCheckBox("Draw SfM points");
    this->render_cb->setChecked(true);
    this->connect(this->render_cb, SIGNAL(clicked()),
        this, SLOT(repaint()));
    this->connect(&SceneManager::get(), SIGNAL(scene_bundle_changed()),
        this, SLOT(reset_scene_bundle()));
    this->connect(&SceneManager::get(), SIGNAL(scene_selected(mve::Scene::Ptr)),
        this, SLOT(reset_scene_bundle()));
}

QWidget*
AddinSfmRenderer::get_sidebar_widget (void)
{
    return this->render_cb;
}

void
AddinSfmRenderer::paint_impl (void)
{
    if (this->render_cb->isChecked())
    {
        /* Try to create renderer. */
        try {
            if (this->sfm_renderer == nullptr)
                this->create_renderer(!this->first_create_renderer);
            this->first_create_renderer = false;

            /* Try to render. */
            if (this->sfm_renderer != nullptr)
            {
                this->state->wireframe_shader->bind();
                this->state->wireframe_shader->send_uniform("ccolor", math::Vec4f(0.0f));
                this->sfm_renderer->draw();
            }
        } catch (...) {
            std::cerr << "Error rendering SfM points\n";
        }
    }
}

void
AddinSfmRenderer::create_renderer (bool raise_error_on_failure)
{
    if  (this->state->scene == nullptr)
        return;

    try
    {
        mve::Bundle::ConstPtr bundle(this->state->scene->get_bundle());
        mve::TriangleMesh::Ptr mesh(bundle->get_features_as_mesh());
        this->sfm_renderer = ogl::MeshRenderer::create(mesh);
        this->sfm_renderer->set_shader(this->state->wireframe_shader);
        this->sfm_renderer->set_primitive(GL_POINTS);
    }
    catch (std::exception& e)
    {
        // Comment this out, as this will will not be used to read bundle files
        // or sfm. Hence, fail quietly.
        this->render_cb->setChecked(false);
        // std::cerr << "Error reading SfM points.\n";
        if (raise_error_on_failure) {
            // this->show_error_box("Error reading bundle", e.what());
        }
        return;
    }
}

void
AddinSfmRenderer::reset_scene_bundle (void)
{
    this->sfm_renderer.reset();
    this->first_create_renderer = true;
}
