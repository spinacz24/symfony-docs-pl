.. index::
   single: trasowanie; znak ukośnika w parametrach

Jak zezwolić na używanie znaku ukośnika w parametrach trasowania
================================================================

Czasami potrzeba tworzyć ścieżki URL z parametremi zawierającymi znak
ukośnika  (``/``). 
Dla przykładu, rozważmy klasyczną trasę ``/hello/{username}``. Domyślnie, ścieżka
``/hello/Fabien`` będzie pasowała do tej trasy, ale już ``/hello/Fabien/Kris`` nie.
Dzieje się tak ponieważ Symfony używa tego znaku jako separatora części trasy.

W tym artkule wyjaśniamy jak można zmodyfikować trasę tak, aby ścieżka
``/hello/Fabien/Kris`` pasowała do routingu ``/helo/{username}``, gdzie ``{username}``
odpowiada ciągowi ``Fabien/Kris``.

Konfiguracja trasowania
-----------------------

Domyślnie, komponent Routing Symfony wymagaja aby parametry 
zgodne były wzorcem wyrazeniem regularnym ``[^/]+``. Oznacza to, że dozwolone
sa wszystkie znaki z wyjątkiem ``/``.

Trzeba jawnie zezwolić, aby znak ``/`` był częścią parametru, określając bardziej
liberalny wzorzec ścieżki.

.. configuration-block::

    .. code-block:: php-annotations
       :linenos:

        use Sensio\Bundle\FrameworkExtraBundle\Configuration\Route;

        class DemoController
        {
            /**
             * @Route("/hello/{username}", name="_hello", requirements={"username"=".+"})
             */
            public function helloAction($username)
            {
                // ...
            }
        }

    .. code-block:: yaml
       :linenos:

        _hello:
            path:     /hello/{username}
            defaults: { _controller: AppBundle:Demo:hello }
            requirements:
                username: .+

    .. code-block:: xml
       :linenos:

        <?xml version="1.0" encoding="UTF-8" ?>

        <routes xmlns="http://symfony.com/schema/routing"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/routing http://symfony.com/schema/routing/routing-1.0.xsd">

            <route id="_hello" path="/hello/{username}">
                <default key="_controller">AppBundle:Demo:hello</default>
                <requirement key="username">.+</requirement>
            </route>
        </routes>

    .. code-block:: php
       :linenos:

        use Symfony\Component\Routing\RouteCollection;
        use Symfony\Component\Routing\Route;

        $collection = new RouteCollection();
        $collection->add('_hello', new Route('/hello/{username}', array(
            '_controller' => 'AppBundle:Demo:hello',
        ), array(
            'username' => '.+',
        )));

        return $collection;

To wszystko! Teraz, parametr ``{username}`` może zawierać znak ``/``.
