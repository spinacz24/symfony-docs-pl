.. index::
    single: treść; pakiety
    single: ContentBundle

ContentBundle
=============

    Pakiet ContentBundle dostarcza dokument dla statycznej treści i kontroler
    do renderowania każdej treści, która wymaga specjalnej obsługi w kontrolerze.

W celu wprowadzenia proszę zapoznać się z artykułem ":doc:`../../book/static_content`"
w rozdziale "Podręcznik CMF Symfony".

Instalacja
----------

Pakiet można zainstalować `poprzez Composer`_ wykorzystując pakiet`symfony-cmf/content-bundle`_.

Stosowanie
----------

Pakiet ContentBundle dostarcza dokument ``StaticContent``, który można użyć dla
treści statycznej. Dokument ten wymaga tytułu i ciała treści i może zostać połączony
do wielu tras i elementów menu. Prostą stronę można utworzyć w taki sposób::

    use Symfony\Cmf\Bundle\ContentBundle\Doctrine\Phpcr\StaticContent;
    use Symfony\Cmf\Bundle\RoutingBundle\Doctrine\Phpcr\Route;

    // retrieve the route root node
    $routeRoot = $documentManager->find(null, '/cms/routes');

    $route = new Route();
    $route->setPosition($routeRoot, 'hello');

    $documentManager->persist($route); // add the route

    // retrieve the content root node
    $contentRoot = $documentManager->find(null, '/cms/content');

    $content = new StaticContent();
    $content->setTitle('Hello World');
    $content->setBody(...);
    $content->addRoute($route);
    $content->setParentDocument($contentRoot);
    $content->setName('hello-world');

    $documentManager->persist($content); // add the content

    $documentManager->flush(); // save changes

Kod ten łączy z sobą trasę ``/hello`` i dokument treści ``hello-world``.
Oznacza to, że odwiedzając ``/hello`` otrzyma się treść zawartą w ``hello-world``.
Jednak przed tym trzeba odpowiednio skonfigurować kontroler.

ContentController
~~~~~~~~~~~~~~~~~

Pakiet ContentBundle dostarcza ``ContentController``.
Kontroler ten może ogólnie obsługiwać przychodzące żądania i przekazywać je do
szablonu. Stosuje się go zazwyczaj
z :ref:`dynamicznym routerem <bundles-routing-dynamic_router-enhancer>`.

Tworzenie szablonu
..................

W celu zrenderowania treści, trzeba utworzyć i skonfigurować szablon.
Można to wykonać używając ustawienia ``templates_by_class`` (patrz niżej) albo
konfigurując domyślny szablon.

Każdy szablon renderowany przez ``ContentController`` będzie przekazywany zmiennej
``cmfMainContent``, która zwiera bieżący dokument ``StaticContent``.

Na przykład, bardzo prosty szablon wygląda tak:

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/BlogBundle/Resources/views/Post/index.html.twig #}
        {% extends '::layout.html.twig' %}

        {% block title -%}
            {{ cmfMainContent.title }}
        {%- endblock %}

        {% block content -%}
            <h1>{{ cmfMainContent.title }}</h1>

            {{ cmfMainContent.body|raw }}
        {%- endblock %}

    .. code-block:: html+php

        <!-- src/Acme/BlogBundle/Resources/views/Post/index.html.php -->
        <?php $view->extend('::layout.html.twig') ?>

        <?php $view['slots']->set('title', $cmfMainContent->getTitle()) ?>

        <?php $view['slots']->start('content') ?>
        <h1><?php echo $cmfMainContent->getTitle() ?></h1>

        <?php echo $cmfMainContent->getBody() ?>
        <?php $view['slots']->stop() ?>

.. _bundles-content-introduction_default-template:

Konfigurowanie domyślnego szablonu
..................................

Dla skonfigurowania domyślnego szablonu trzeba użyć opcji ``default_template``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml

        # ...
        cmf_content:
            default_template: AcmeBlogBundle:Content:static.html.twig

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <!-- ... -->

            <config xmlns="http://cmf.symfony.com/schema/dic/content"
                default-template="AcmeMainBundle:Content:static.html.twig"
            />
        </container>

    .. code-block:: php

        // app/config/config.yml

        // ...
        $container->loadFromExtension('cmf_content', array(
            'default_template' => 'AcmeMainBundle:Content:static.html.twig',
        ));

Teraz zostanie użyty ten szablon, ilekroć kontroler treści zostanie wywołany bez
określenia szablonu.

Ustawienie trasowania
---------------------

Pakiet RoutingBundle zapewnia wydajne narzędzia do konfigurowania tego, jak mogą
być odwzorowywane do kontrolerów i szablonów dynamiczne trasy  i ich treść.

Załóżmy, że chcemy obsłużyć dokument ``StaticContent`` z domyślnym kontrolerem
``ContentController``. Aby to osiągnąć, trzeba użyć opcji konfiguracyjnej
``cmf_routing.dynamic.controllers_by_class``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml

        # ...
        cmf_routing:
            dynamic:
                controllers_by_class:
                    Symfony\Cmf\Bundle\ContentBundle\Doctrine\Phpcr\StaticContent: cmf_content.controller:indexAction

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services">

            <!-- ... -->

            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <dynamic>
                    <controller-by-class
                        class="Symfony\Cmf\Bundle\ContentBundle\Doctrine\Phpcr\StaticContent">
                        cmf_content.controller:indexAction
                    </controller-by-class>
        </container>

    .. code-block:: php

        // app/config/config.yml

        // ...
        $container->loadFromExtension('cmf_routing', array(
            'dynamic' => array(
                'controller_by_class' => array(
                    'Symfony\Cmf\Bundle\ContentBundle\Doctrine\Phpcr\StaticContent' => 'cmf_content.controller:indexAction',
                ),
            ),
        ));

Teraz wszystko zostało odpowiednio skonfigurowane. Po przejściu do ``/hello``
na stronie wyświetli się nasza treść.

Wykorzystywanie templates_by_class
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jest to powszechnie stosowany sposób na przypisywanie szablonu do treści, zamiast
wykorzystywania domyślnego szablonu. Dzięki niemu można mieć różne szablony dla
różnych dokumentów w celu obsługi ich określonych właściwości lub wytwarzania
indywidualnego kodu HTML. Dla odwzorowania szablonu na treść, trzeba użyć opcji
``templates_by_class``. Gdy szablon zostanie odnaleziony w ten sposób, do
renderowania treści zostanie wykorzystany ogólny kontroler, którym domyślnie jest
``ContentController``.

.. tip::

    Pakiet trasowania zapewnia wiele zaawansowanych funkcji do konfigurowania
    odwzorowywanych kontrolerów i szablonów. Proszę przeczytać więcej na ten
    temat w   :ref:`informatorze konfiguracji trasowania
    <reference-config-routing-template_by_class>`.

Integracja z pakietem SonataAdminBundle
---------------------------------------

Pakiet ContentBundle dostarcza również klasę Admin do udostępnienia tworzenia,
edytowania i usuwania statycznej treści z panelu administracyjnego. W celu
udostępnienia administratora trzeba wykorzystać ustawienie
``cmf_content.persistence.phpcr.use_sonata_admin``. Pakiet CoreBundle CMF zapewnia
również :ref:`kilka przydatnych rozszerzeń <bundles-core-persistence>` dla SonataAdminBundle.

.. _`poprzez Composer`: http://getcomposer.org
.. _`symfony-cmf/content-bundle`: https://packagist.org/packages/symfony-cmf/content-bundle
