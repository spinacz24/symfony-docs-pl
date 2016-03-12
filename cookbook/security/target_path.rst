.. index::
   single: bezpieczeństwo; ścieżka przekierowania

Jak zmienić domyślne zachowanie metody setTargetPath
====================================================

Domyślnie, komponent Security zachowuje informacje o ścieżce URI z ostaniego żądania
w zmiennej sesji o nazwie ``_security.main.target_path`` (gdzie ``main`` jest
nazwą zapory, zdefiniowaną w ``security.yml``). Po udanym logowaniu, użytkownik
zostaje przekierowany na ta ścieżkę, co ma pomóc w odszukaniu ostatnio odwiedzanej
strony.

W niektórych przypadkach mie jest to dobre rozwiązanie. Na przykład, gdy
identyfikatorem URI ostatniego żądania jest obiekt XMLHttpRequest, zwracający
odpowiedź nie będącą kodem HTML lub będącą tylko częściowo kodem HTML, użytkownik
zostaje przekierowany do strony, którą przeglądarka nie może zrenderować.

W celu obejścia tego problemu, trzeba po prostu rozszerzyć klasę ``ExceptionListener``
i nadpisać domyślną metodę o nazwie ``setTargetPath()``.

Najpierw, przesłońmy parametr ``security.exception_listener.class`` w pliku
konfiguracyjnym. Można to zrobić w pliku głównej konfiguracji (``app/config``)
lub pliku konfiguracyjnym importowanym z pakietu:

.. configuration-block::

        .. code-block:: yaml
           :linenos:

            # app/config/services.yml
            parameters:
                # ...
                security.exception_listener.class: AppBundle\Security\Firewall\ExceptionListener

        .. code-block:: xml
           :linenos:

            <!-- app/config/services.xml -->
            <parameters>
                <!-- ... -->
                <parameter key="security.exception_listener.class">AppBundle\Security\Firewall\ExceptionListener</parameter>
            </parameters>

        .. code-block:: php
           :linenos:

            // app/config/services.php
            // ...
            $container->setParameter('security.exception_listener.class', 'AppBundle\Security\Firewall\ExceptionListener');

Następnie, utwórzmy własną klasę ``ExceptionListener``::

    // src/AppBundle/Security/Firewall/ExceptionListener.php
    namespace AppBundle\Security\Firewall;

    use Symfony\Component\HttpFoundation\Request;
    use Symfony\Component\Security\Http\Firewall\ExceptionListener as BaseExceptionListener;

    class ExceptionListener extends BaseExceptionListener
    {
        protected function setTargetPath(Request $request)
        {
            // Do not save target path for XHR requests
            // You can add any more logic here you want
            // Note that non-GET requests are already ignored
            if ($request->isXmlHttpRequest()) {
                return;
            }

            parent::setTargetPath($request);
        }
    }

Można tu dodać jakąś logikę, zgodnie z potrzebą przyjętego scenariusza.
