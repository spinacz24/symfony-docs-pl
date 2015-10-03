.. index::
   single: kontroler; jako usługa

Jak zdefiniować kontroler jako usługę
=====================================

W podręczniku zostało wyjaśnione, jak można łatwo użyć kontrolera, gdy rozszerza
on bazową klasę :class:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller`.
Chociaż działa to dobrze, kontrolery można również określić jako usługi, co w wielu
przypadkach jest jeszcze lepsze.

.. note::

    Określnie kontrolera jako usługi wymaga trochę więcej pracy. Główną zaletą
    jest to, że cały kontroler lub wszystkie usługi przekazane do tego kontrolera
    mogą być zmieniane poprzez konfigurację kontenera usług.
    Jest to szczególnie przydatne przy opracowywaniu otwartych pakietów lub
    jakichkolwiek pakietów, które mają być wykorzystane w różnych projektach.

    Drugą korzyścią jest to, że kontrolery są wówczas bardziej przydatne dla
    testowania.
    Patrząc na argumenty konstruktora, łatwo zrozumieć, jakiego typu rzeczy ten
    kontroler może lub nie może wykonywac. Ponieważ każda zależność musi być
    wstrzykiwana ręcznie, więc ta zaleta staje się bardziej oczywista (tj. jeśli
    ma się wiele argumentów konstruktora) gdy kontroler staje sie coraz większy.
    Ważne jest również zalecenie :doc:`najlepszych praktyk </best_practices/controllers>`:
    należy unikać umieszczania logiki biznesowej w kontrolerach. Zamiast tego trzeba
    wstrzykiwać usługi, które wykonują większość pracy.

    Tak więc, jeśli nawet nie określi się swoich kontrolerów jako usługi, to często
    zobaczy się to w otwartych pakietach Symfony. Jest również ważne zrozumienie
    zalet i wad każdego z rozwiązań.

Definowanie kontrolera jako usługi
----------------------------------

Kontroler moze zostać zdefiniowany jako usługa, tak samo jak inne klasy.
Na przykład, jeśli ma się następujący kontroler::

    // src/AppBundle/Controller/HelloController.php
    namespace AppBundle\Controller;

    use Symfony\Component\HttpFoundation\Response;

    class HelloController
    {
        public function indexAction($name)
        {
            return new Response('<html><body>Hello '.$name.'!</body></html>');
        }
    }

to można zdefiniować go jako usługę w ten sposób:

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yml
        services:
            app.hello_controller:
                class: AppBundle\Controller\HelloController

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <services>
            <service id="app.hello_controller" class="AppBundle\Controller\HelloController" />
        </services>

    .. code-block:: php

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;

        $container->setDefinition('app.hello_controller', new Definition(
            'AppBundle\Controller\HelloController'
        ));

Odwoływanie się do usługi
-------------------------

W celu odwołania sie do kontrolera zdefiniowanego jako usługa, trzeba uzyć notacji
z jednym dwukropkiem (:). Na przykład, dokonamy przekazania do metody ``indexAction()``
w usłudze zdefiniowanej powyżej o identyfikatorze ``app.hello_controller``::

    $this->forward('app.hello_controller:indexAction', array('name' => $name));

.. note::

    Używając tą składnię nie można usunąć słowa ``Action`` z nazwy metody.

Mozna również dokonać trasowania do usługi przez uzycie tej samej notacji, gdy
zdefiniowało się wartość trasy jako ``_controller``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/routing.yml
        hello:
            path:     /hello
            defaults: { _controller: app.hello_controller:indexAction }

    .. code-block:: xml

        <!-- app/config/routing.xml -->
        <route id="hello" path="/hello">
            <default key="_controller">app.hello_controller:indexAction</default>
        </route>

    .. code-block:: php

        // app/config/routing.php
        $collection->add('hello', new Route('/hello', array(
            '_controller' => 'app.hello_controller:indexAction',
        )));

.. tip::

    Można też używać adnotacji do konfigurowania trasowania w kontrolerze
    zdefiniowanym jako usługa. Szczegóły można znaleźć w 
    `dokumentacji FrameworkExtraBundle`_.

.. tip::

    Jeśli kontroler implementuje metodę ``__invoke()`` method, można po prostu
    odwołać się do identyfikatora usługi (``app.hello_controller``).

    .. versionadded:: 2.6
        Obsługa metody ``__invoke()`` została wprowadzona w Symfony 2.6.

Alternatywy dla metod bazowego kontrolera
-----------------------------------------

When using a controller defined as a service, it will most likely not extend
the base ``Controller`` class. Instead of relying on its shortcut methods,
you'll interact directly with the services that you need. Fortunately, this is
usually pretty easy and the base `Controller class source code`_ is a great
source on how to perform many common tasks.

For example, if you want to render a template instead of creating the ``Response``
object directly, then your code would look like this if you were extending
Symfony's base controller::

    // src/AppBundle/Controller/HelloController.php
    namespace AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Controller\Controller;

    class HelloController extends Controller
    {
        public function indexAction($name)
        {
            return $this->render(
                'AppBundle:Hello:index.html.twig',
                array('name' => $name)
            );
        }
    }

If you look at the source code for the ``render`` function in Symfony's
`base Controller class`_, you'll see that this method actually uses the
``templating`` service::

    public function render($view, array $parameters = array(), Response $response = null)
    {
        return $this->container->get('templating')->renderResponse($view, $parameters, $response);
    }

In a controller that's defined as a service, you can instead inject the ``templating``
service and use it directly::

    // src/AppBundle/Controller/HelloController.php
    namespace AppBundle\Controller;

    use Symfony\Bundle\FrameworkBundle\Templating\EngineInterface;
    use Symfony\Component\HttpFoundation\Response;

    class HelloController
    {
        private $templating;

        public function __construct(EngineInterface $templating)
        {
            $this->templating = $templating;
        }

        public function indexAction($name)
        {
            return $this->templating->renderResponse(
                'AppBundle:Hello:index.html.twig',
                array('name' => $name)
            );
        }
    }

The service definition also needs modifying to specify the constructor
argument:

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yml
        services:
            app.hello_controller:
                class:     AppBundle\Controller\HelloController
                arguments: ["@templating"]

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <services>
            <service id="app.hello_controller" class="AppBundle\Controller\HelloController">
                <argument type="service" id="templating"/>
            </service>
        </services>

    .. code-block:: php

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $container->setDefinition('app.hello_controller', new Definition(
            'AppBundle\Controller\HelloController',
            array(new Reference('templating'))
        ));

Rather than fetching the ``templating`` service from the container, you can
inject *only* the exact service(s) that you need directly into the controller.

.. note::

   This does not mean that you cannot extend these controllers from your own
   base controller. The move away from the standard base controller is because
   its helper methods rely on having the container available which is not
   the case for controllers that are defined as services. It may be a good
   idea to extract common code into a service that's injected rather than
   place that code into a base controller that you extend. Both approaches
   are valid, exactly how you want to organize your reusable code is up to
   you.

Metody bazowego kontrolera i ich zamienniki w usłudze
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This list explains how to replace the convenience methods of the base
controller:

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::createForm` (service: ``form.factory``)
    .. code-block:: php

        $formFactory->create($type, $data, $options);

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::createFormBuilder` (service: ``form.factory``)
    .. code-block:: php

        $formFactory->createBuilder('form', $data, $options);

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::createNotFoundException`
    .. code-block:: php

        new NotFoundHttpException($message, $previous);

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::forward` (service: ``http_kernel``)
    .. code-block:: php

        use Symfony\Component\HttpKernel\HttpKernelInterface;
        // ...

        $request = ...;
        $attributes = array_merge($path, array('_controller' => $controller));
        $subRequest = $request->duplicate($query, null, $attributes);
        $httpKernel->handle($subRequest, HttpKernelInterface::SUB_REQUEST);

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::generateUrl` (service: ``router``)
    .. code-block:: php

       $router->generate($route, $params, $absolute);

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::getDoctrine` (service: ``doctrine``)

    *Simply inject doctrine instead of fetching it from the container*

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::getUser` (service: ``security.token_storage``)
    .. code-block:: php

        $user = null;
        $token = $tokenStorage->getToken();
        if (null !== $token && is_object($token->getUser())) {
             $user = $token->getUser();
        }

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::isGranted` (service: ``security.authorization_checker``)
    .. code-block:: php

        $authChecker->isGranted($attributes, $object);

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::redirect`
    .. code-block:: php

        use Symfony\Component\HttpFoundation\RedirectResponse;

        return new RedirectResponse($url, $status);

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::render` (service: ``templating``)
    .. code-block:: php

        $templating->renderResponse($view, $parameters, $response);

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::renderView` (service: ``templating``)
    .. code-block:: php

       $templating->render($view, $parameters);

:method:`Symfony\\Bundle\\FrameworkBundle\\Controller\\Controller::stream` (service: ``templating``)
    .. code-block:: php

        use Symfony\Component\HttpFoundation\StreamedResponse;

        $templating = $this->templating;
        $callback = function () use ($templating, $view, $parameters) {
            $templating->stream($view, $parameters);
        }

        return new StreamedResponse($callback);

.. tip::

    ``getRequest`` has been deprecated. Instead, have an argument to your
    controller action method called ``Request $request``. The order of the
    parameters is not important, but the typehint must be provided.

.. _`Controller class source code`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/Controller.php
.. _`base Controller class`: https://github.com/symfony/symfony/blob/master/src/Symfony/Bundle/FrameworkBundle/Controller/Controller.php
.. _`dokumentacji FrameworkExtraBundle`: https://symfony.com/doc/current/bundles/SensioFrameworkExtraBundle/annotations/routing.html
