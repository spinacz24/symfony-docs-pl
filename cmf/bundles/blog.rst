.. highlight:: php
   :linenothreshold: 2

.. index::
    single: blog; pakiety
    single: BlogBundle

BlogBundle
==========

.. include:: _outdate-caution.rst.inc

.. include:: _not-stable-caution.rst.inc

Pakiet ten ma zapewnić dostarczenie wszystkiego, co potrzebne jest to stworzenia
pełnego blogu lub systemu podobnego do blogu. Zawiera również wbudowaną obsługę
pakietu Sonata Admin, aby pomóc w pobraniu i szybkim uruchomieniu aplikacji.

Obecne funkcje:

* obsługa wielu blogów w ramach jednej witryny;
* umieszczenie blogów w dowolnym miejscu w hierarchii tras;
* integracja z Sonata Admin.

Projektowane funkcje:

* pełna obsługa tagów;
* frontonowe stronicowanie (przy użyciu knp-paginator);
* kanały informacyjne RSS/ATOM;
* komentarze;
* obsługa użytkowników (FOSUserBundle).

Uwagi o tym dokumencie
----------------------

* Przykłady konfiguracji XML mogą być sformatowane niepoprawnie.

Zależności
----------

* :doc:`CmfRoutingBundle <routing/introduction>` jest używany do zarządzania trasowaniem;
* :doc:`CmfRoutingAutoBundle <routing_auto/introduction>` jest używany do zarządzania automatycznym generowaniem tras;
* :doc:`PHPCR-ODM <phpcr_odm/introduction>` jest używany do utrzymania dokumentów pakietów.

Konfiguracja
------------

Przykład:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config.yml
        cmf_blog:
            use_sonata_admin: auto
            blog_basepath: /cms/blog
            class:
                blog_admin: Symfony\Cmf\Bundle\BlogBundle\Admin\BlogAdmin # Optional
                post_admin: Symfony\Cmf\Bundle\BlogBundle\Admin\PostAdmin # Optional
                blog: Symfony\Cmf\Bundle\BlogBundle\Document\Blog # Optional
                post: Symfony\Cmf\Bundle\BlogBundle\Document\Post # Optional

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <config xmlns="http://cmf.symfony.com/schema/dic/blog"
            use-sonata-admin="auto"
            blog-basepath="/cms/blog"
        >
            <class
                blog-admin="Symfony\Cmf\Bundle\BlogBundle\Admin\BlogAdmin"
                post-admin="Symfony\Cmf\Bundle\BlogBundle\Admin\PostAdmin"
                blog="Symfony\Cmf\Bundle\BlogBundle\Document\Blog"
                post="Symfony\Cmf\Bundle\BlogBundle\Document\Post"
            />
        </config>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('cmf_blog', array(
            'use_sonata_admin' => 'auto',
            'blog_basepath' => '/cms/blog',
            'class' => array(
                'blog_admin' => 'Symfony\Cmf\Bundle\BlogBundle\Admin\BlogAdmin', // optional
                'post_admin' => 'Symfony\Cmf\Bundle\BlogBundle\Admin\PostAdmin', // optional
                'blog' => 'Symfony\Cmf\Bundle\BlogBundle\Document\Blog', // optional
                'post' => 'Symfony\Cmf\Bundle\BlogBundle\Document\Post', // optional
            ),
        ));

Wyjaśnienie:

* **use_sonata_admin** - określa, czy próbować integracji z Sonata Admin;
* **blog_basepath** - (*wymagane*) określa ścieżkę miejsca, w którym ma być
  przechowywana treść blogu, używana przez Sonata Admin;
* **class** - pozwala określić własne klasy dla Sonata Admin i dokumentów;
  * **blog_admin**: FQN klasy Sonata Admin do stosowania w zarządzaniu blogami;
  * **post_admin**: FQN klasy Sonata Admin do zastosowania przy zarządzania wpisami;
  * **blog**: FQN klasy dokumentu, który Sonata Admin będzie stosował dla blogów;
  * **post**: FQN klasy dokumentu, który Sonata Admin będzie stosował dla wpisów;.

.. note::

    Jeśli zmieni się domyślne dokumenty **trzeba** zaktualizować konfigurację
    automatycznego trasowania , gdyż system automatycznego trasowania nie rozpozna
    nowych klas i w konsekwencji nie będzie generował tras.

Automatyczne trasowanie
~~~~~~~~~~~~~~~~~~~~~~~

Pakiet blogu używa pakietu ``CmfRoutingAuto`` do generowania tras dla poszczególnych
treści. Trzeba będzie skonfigurować automatyczne trasowanie, aby to działało.

Można zawrzeć wartość domyślną w głównym pliku konfiguracji, jak niżej:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        imports:
            # ...
            - { resource: @CmfBlogBundle/Resources/config/routing/autoroute_default.yml }
        # ...

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <imports>
            <!-- ... -->
            <import resource="@CmfBlogBundle/Resources/config/routing/autoroute_default" />
        </imports>
        <!-- ... -->

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $loader->import('config.php');
        // ...

Domyślna konfiguracja będzie wytwarzać adresy URL podobne do tego::

    http://www.example.com/blogs/dtls-blog/2013-04-14/this-is-my-post

Prosimy o zapoznanie się z dokumentacją :doc:`RoutingAutoBundle <routing_auto/introduction>`
w celu uzyskania więcej informacji.

Trasowanie treści
~~~~~~~~~~~~~~~~~

Dla włączenia systemu trasowania w tryb automatycznego przekazywania żądań do
kontrolera blogu, gdy treść ``Blog`` lub ``Post``  zostaje związana z trasą,
dodajmy następujący kod w podsekcji sekcji ``controllers_by_class`` sekcji
``cmf_routing_extra`` w głównym pliku konfiguracyjnym:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        cmf_routing_extra:
            # ...
            dynamic:
                # ...
                controllers_by_class:
                    # ...
                    Symfony\Cmf\Bundle\BlogBundle\Document\Blog: cmf_blog.blog_controller:listAction
                    Symfony\Cmf\Bundle\BlogBundle\Document\Post: cmf_blog.blog_controller:viewPostAction

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <config xmlns="http://cmf.symfony.com/schema/dic/blog">
            <dynamic>
                <controllers-by-class
                    class="Symfony\CmfBundle\BlogBundle\Document\Post"
                >
                    cmf_blog.blog_controller:listAction"
                </controllers-by-class>
            </dynamic>
        </config>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('cmf_routing_extra', array(
            // ...
            'dynamic' => array(
                'controllers_by_class' => array(
                    'Symfony\Cmf\Bundle\BlogBundle\Document\Blog' => 'cmf_blog.blog_controller:listAction',
                    'Symfony\Cmf\Bundle\BlogBundle\Document\Post' => 'cmf_blog.blog_controller:viewPostAction',
                ),
            ),
        ));

Sonata Admin
~~~~~~~~~~~~

``BlogBundle`` ma usługi administracyjne zdefiniowane dla Sonata Admin, sprawiające,
że system blogu jest widoczny na pulpicie. Dodajmy następujący kod do sekcji
``sonata_admin``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        sonata_admin:
            # ...
            dashboard:
                groups:
                    # ...
                    blog:
                        label: blog
                        items:
                            - cmf_blog.admin
                            - cmf_post.admin

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
        <config xmlns="http://example.org/schema/dic/sonata_admin">
            <!-- ... -->

            <dashboard>
                <groups id="blog"
                    label="blog">
                    <item>cmf_blog.admin</item>
                    <item>cmf_post.admin</item>
                </groups>
            </dashboard>
        </config>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('sonata_admin', array(
            // ...
            'dashboard' => array(
                'groups' => array(
                    // ...
                    'blog' => array(
                        'label' => 'blog',
                        'items' => array(
                            'cmf_blog.admin',
                            'cmf_post.admin',
                        ),
                    ),
                ),
            ),
        ));

Pakiet przeglądarki drzewa
~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeśli używa się przeglądarki drzewa CMF Symfon, to można eksponować trasy blogu,
aby włączyć edytowanie blogu z poziomu przeglądarki drzewa. Eksponujmy trasy w sekcji
``fos_js_routing`` pliku konfiguracyjnego:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        fos_js_routing:
            routes_to_expose:
                # ...
                - admin_bundle_blog_blog_create
                - admin_bundle_blog_blog_delete
                - admin_bundle_blog_blog_edit

    .. code-block:: xml
       :linenos;

        <!-- app/config/config.xml -->
        <config xmlns="http://example.org/schema/dic/fos_js_routing">
            <!-- ... -->
            <routes-to-expose>admin_bundle_blog_blog_create</routes-to-expose>
            <routes-to-expose>admin_bundle_blog_blog_delete</routes-to-expose>
            <routes-to-expose>admin_bundle_blog_blog_edit</routes-to-expose>
        </config>

    .. code-block:: php
       :linenos;

        // app/config/config.php
        $container->loadFromExtension('fos_js_routing', array(
            'routes_to_expose' => array(
                // ...
                'admin_bundle_blog_blog_create',
                'admin_bundle_blog_blog_delete',
                'admin_bundle_blog_blog_edit',
        )));

Integracja
----------

Szablonowanie
~~~~~~~~~~~~~

Domyślne szablony są oznakowane dla `Twitter Bootstrap`_. Lecz prostą rzeczą jest
całkowite dostosowanie szablonów przez ich **zastąpienie**.

Szablon, który trzeba będzie zastąpić, to domyślny układ. Trzeba go zmienić i wykonać
rozszerzenie układu aplikacji. Najprościej jest utworzyć następujący plik:

.. configuration-block::

    .. code-block:: jinja
       :linenos;

        {# app/Resources/CmfBlogBundle/views/default_layout.html.twig #}
        {% extends "MyApplicationBundle::my_layout.html.twig" %}

        {% block content %}
        {% endblock %}

    .. code-block:: php
       :linenos;

        <!-- app/Resources/CmfBlogBundle/views/default_layout.html.twig -->
        <?php $view->extend('MyApplicationBundle::my_layout.html.twig') ?>

        <?php $view['slots']->output('content') ?>

Blog będzie teraz uzywać ``MyApplicationBundle::my_layout.html.twig`` zamiast
``CmfBlogBundle::default_layout.html.twig``.

Przeczytaj artykuł `Nadpisywanie szablonów pakietów`_ w dokumentacji Symfony
w celu poznania więcej szczegółów.

.. _`controllers as services`: http://symfony.com/doc/current/cookbook/controller/service.html
.. _`Twitter Bootstrap`: http://twitter.github.com/bootstrap/
.. _`Overriding Bundle Templates`: http://symfony.com/doc/current/book/templating.html#overriding-bundle-templates
