Kontrolery i szablony
---------------------

Przejdź w przeglądarce do adresu http://localhost:8000/page/home – powinna to być
Twoja strona, lecz jest na niej komunikat mówiący, że nie można znaleźć kontrolera.
Innymi słowami Symfony odnalazł *trasę odpowiedniej strony* ale nie wie co zrobić
z tą stroną.

Można odwzorować domyślny kontroler dla wszystkich instancji ``Page``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        cmf_routing:
            dynamic:
                # ...
                controllers_by_class:
                    Acme\BasicCmsBundle\Document\Page: Acme\BasicCmsBundle\Controller\DefaultController::pageAction

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>

        <container xmlns="http://cmf.symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

            <config xmlns="http://cmf.symfony.com/schema/dic/routing">
                <dynamic generic-controller="cmf_content.controller:indexAction">
                    <!-- ... -->
                    <controllers-by-class
                        class="Acme\BasicCmsBundle\Document\Page"
                    >
                        Acme\BasicCmsBundle\Controller\DefaultController::pageAction
                    </controllers-by-class>
                </dynamic>
            </config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('cmf_routing', array(
            'dynamic' => array(
                // ...
                'controllers_by_class' => array(
                    'Acme\BasicCmsBundle\Document\Page' => 'Acme\BasicCmsBundle\Controller\DefaultController::pageAction',
                ),
            ),
        ));

Spowoduje to, że żądania będą przekazywane do tego kontrolera, gdy trasa, która
dopasowuje przychodzące żądania jest dostarczana przez dynamiczny router i dokument
treści, taki że przywoływana trasa jest klasą ``Acme\BasicCmsBundle\Document\Page``.

Teraz utworzymy akcje w domyślnym kontrolerze – można przekazać do widoku obiekt
``Page`` i wszystkie obiekty ``Posts``::

    // src/Acme/BasicCmsBundle/Controller/DefaultController.php

    // ...
    class DefaultController extends Controller
    {
        // ...

        /**
         * @Template()
         */
        public function pageAction($contentDocument)
        {
            $dm = $this->get('doctrine_phpcr')->getManagerForClass('AcmeBasicCmsBundle:Post');
            $posts = $dm->getRepository('AcmeBasicCmsBundle:Post')->findAll();

            return array(
                'page'  => $contentDocument,
                'posts' => $posts,
            );
        }
    }

Obiekt ``Page`` jest przekazywany jako ``$contentDocument``.

Dodamy odpowiedni szablon Twiga (zwróć uwagę, że to działa, bo użyliśmy adnotacji
``@Template``):

.. configuration-block::

    .. code-block:: html+jinja

        {# src/Acme/BasicCmsBundle/Resources/views/Default/page.html.twig #}
        <h1>{{ page.title }}</h1>
        <p>{{ page.content|raw }}</p>
        <h2>Our Blog Posts</h2>
        <ul>
            {% for post in posts %}
                <li><a href="{{ path(post) }}">{{ post.title }}</a></li>
            {% endfor %}
        </ul>

    .. code-block:: html+php

        <!-- src/Acme/BasicCmsBundle/Resources/views/Default/page.html.twig -->
        <h1><?php echo $page->getTitle() ?></h1>
        <p><?php echo $page->getContent() ?></p>
        <h2>Our Blog Posts</h2>
        <ul>
            <?php foreach($posts as $post) : ?>
                <li>
                    <a href="<?php echo $view['router']->generate($post) ?>">
                        <?php echo $post->getTitle() ?>
                    </a>
                </li>
            <?php endforeach ?>
        </ul>

Teraz jeszcze jedne odwiedziny pod adresem http://localhost:8000/page/home.

Zwróć uwagę na to, co się dzieje z obiektem wpisu i na funkcję ``path``.
Przekazujemy obiekt ``Post`` i funkcję ``path``, które będą przekazane do obiektu
w routerze i dlatego implementuje to ``RouteReferrersReadInterface``.
``DynamicRouter`` będzie w stanie generować ścieżkę URL dla wpisu.


Kliknij na ``Post`` a otrzymasz ten sam błąd, który wystąpił przed wyświetleniem
strony ``/home`` i można to rozwiązać w ten sam sposób.

.. tip::

    Jeśli ma się różne klasy treści z różnych szablonów, ale nie potrzebuje się
    określonej logiki kontrolera, to można skonfigurować ``templates_by_class``
    zamiast ``controllers_by_class``, aby pozwolić domyślnemu kontrolerowi renderować
    określony szablon. Zapoznaj się z :ref:`bundles-routing-dynamic_router-enhancer`
    w celu uzyskania więcej informacji.
