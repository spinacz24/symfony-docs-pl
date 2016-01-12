.. index::
   single: wiadomości email; w trakcie programowania


Jak pracować z pocztą elektroniczną podczas programowania
=========================================================

Kiedy tworzysz aplikację wysyłającą wiadomości email, raczej nie będziesz chciał
ich wysyłać do rzeczywistych odbiorców w czasie prac programistycznych.
Jeśli w Symfony używa się SwiftmailerBundle, to możesz łatwo osiągnąć ten cel
poprzez odpowiednie ustawienia konfiguracyjne, bez konieczności wprowadzania
zmian w kodzie.
Istnieją dwie podstawowe możliwości obsługi poczty elektronicznej podczas programowania:
(a) całkowite wyłączenie wysyłania wiadomości email lub (b) wysyłanie wszystkich
wiadomości na specjalny adres.

Wyłączenie wysyłania
--------------------

Można wyłączyć wysyłanie wiadomości email poprzez ustawienie opcji
``disable_delivery`` na ``true``.
Jest to domyślna wartośc w środowisku ``test`` w Symfony Standard Edition.
Jeśli ustawi sie tą opcję w pliku konfiguracyjnym ``test``, wiadomości nie
będą wysyłane podczas wywoływania testów, ale będą wysyłane w środowisku ``prod``
oraz ``dev``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config_test.yml
        swiftmailer:
            disable_delivery:  true

    .. code-block:: xml
       :linenos:

        <!-- app/config/config_test.xml -->

        <!--
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            http://symfony.com/schema/dic/swiftmailer http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd
        -->

        <swiftmailer:config
            disable-delivery="true" />

    .. code-block:: php
       :linenos:

        // app/config/config_test.php
        $container->loadFromExtension('swiftmailer', array(
            'disable_delivery'  => "true",
        ));

Jeśli chce się także wyłączyć dostarczanie wiadomości email w środowisku ``dev``, 
po prostu, trzeba to ustawić w pliku ``config_dev.yml``.

Wysyłanie na specjalny adres
----------------------------

Można też spowodować, że wszystkie wiadomości będą wysyłane na specjalny
adres testowy, zamiast adresu faktycznie podanego w wiadomości.
Robi się to ustawiając odpowiednio opcje ``delivery_address``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config_dev.yml
        swiftmailer:
            delivery_address:  dev@example.com

    .. code-block:: xml
       :linenos:

        <!-- app/config/config_dev.xml -->

        <!--
        xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
        http://symfony.com/schema/dic/swiftmailer http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd
        -->

        <swiftmailer:config
            delivery-address="dev@example.com" />

    .. code-block:: php
       :linenos:

        // app/config/config_dev.php
        $container->loadFromExtension('swiftmailer', array(
            'delivery_address'  => "dev@example.com",
        ));

Wyobrażmy sobie teraz, że wysyła się wiadomość na adres ``recipient@example.com``:

.. code-block:: php
   :linenos:

    public function indexAction($name)
    {
        $message = \Swift_Message::newInstance()
            ->setSubject('Hello Email')
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody($this->renderView('HelloBundle:Hello:email.txt.twig', array('name' => $name)))
        ;
        $this->get('mailer')->send($message);

        return $this->render(...);
    }

W środowisku ``dev`` wiadomość zostanie wysłana na adres ``dev@example.com``.
Swiftmailer doda także do wiadomości dodatkowy nagłówek ``X-Swift-To``, zawierający
zamieniony adres, dzięki czemu będzie można nadal sprawdzać do kogo wiadomość
została dostarczona.

.. note::

    Oprócz adresu ``to``, opcja ta zaprzestanie wysyłania wiadomości do ustawionych
    adresów ``CC`` oraz ``BCC``. Swiftmailer doda do wiadomości dodatkowe nagłówki
    z nadpisanymi adresami.
    Są to ``X-Swift-Cc`` oraz ``X-Swift-Bcc`` dla wiadomości ``CC`` i ``BCC``.

.. _sending-to-a-specified-address-but-with-exceptions:

Wysyłanie na specjalny adres, ale z wyjątkami
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Załóżmy, że chcemy mieć wszystkie wiadomości przekierowane na specjalny adres
(podobnie jak w powyższym scenariuszu do ``dev@example.com``), ale też chcemy
wysłać wiadomość na kilka rzeczywistych adresów, aby zbadać działanie poczty
bez przekierowań (nawet jeśłi jest to środowisko dev). Można to zrobić dodając
opcję ``delivery_whitelist``:

.. configuration-block::

    .. code-block:: yaml
       :linenos: 

        # app/config/config_dev.yml
        swiftmailer:
            delivery_address: dev@example.com
            delivery_whitelist:
               # all email addresses matching this regex will *not* be
               # redirected to dev@example.com
               - "/@specialdomain.com$/"

               # all emails sent to admin@mydomain.com won't
               # be redirected to dev@example.com too
               - "/^admin@mydomain.com$/"

    .. code-block:: xml
       :linenos:

        <!-- app/config/config_dev.xml -->

        <?xml version="1.0" charset="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer">

        <swiftmailer:config delivery-address="dev@example.com">
            <!-- all email addresses matching this regex will *not* be redirected to dev@example.com -->
            <swiftmailer:delivery-whitelist-pattern>/@specialdomain.com$/</swiftmailer:delivery-whitelist-pattern>

            <!-- all emails sent to admin@mydomain.com won't be redirected to dev@example.com too -->
            <swiftmailer:delivery-whitelist-pattern>/^admin@mydomain.com$/</swiftmailer:delivery-whitelist-pattern>
        </swiftmailer:config>

    .. code-block:: php 
       :linenos:

        // app/config/config_dev.php
        $container->loadFromExtension('swiftmailer', array(
            'delivery_address'  => "dev@example.com",
            'delivery_whitelist' => array(
                // all email addresses matching this regex will *not* be
                // redirected to dev@example.com
                '/@specialdomain.com$/',

                // all emails sent to admin@mydomain.com won't be
                // redirected to dev@example.com too
                '/^admin@mydomain.com$/',
            ),
        ));

W powyższym przykladzie, wszystkie wiadomości email zostaną przekierowane na
``dev@example.com``, z wyjatkiem wiadomości wysłanych na adres ``admin@mydomain.com``
na jakikolwiek adres email należący do domeny ``specialdomain.com``, które to wiadomości
są dostarczane normalnie.

Podgląd na pasku narzędziowym debugowania
-----------------------------------------

Jeśli jest się w środowisku ``dev``, to na pasku narzędziowym debugowania można
zobaczyć specyfikację każdej wiadomości wysłanej podczas jednej odpowiedzi.
Ikona e-mail na pasku narzędzi informuje ile wiadomości zostało wysłanych. Jeśli
się ją kliknie, zobaczy się raport z dokładniejszymi informacjami.

Jeśli wysyła się wiadomość  i następnie następuje natychmiastowe przekierowanie
do innej strony, to na pasku narzedziowym debugowania nie zostanie wyświetlona
ikona wiadomości email lub raport o nastęþnej stronie.

Rozwiązaniem jest ustawienie w pliku ``config_dev.yml`` opcji ``intercept_redirects``
na ``true``, co spowoduje zatrzymanie przekierowania na inna stronę i umożliwi
otworzenie raportu ze szczegółowymi informacjami o wysłanych wiadomościach.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config_dev.yml
        web_profiler:
            intercept_redirects: true

    .. code-block:: xml
       :linenos:

        <!-- app/config/config_dev.xml -->

        <!--
            xmlns:webprofiler="http://symfony.com/schema/dic/webprofiler"
            xsi:schemaLocation="http://symfony.com/schema/dic/webprofiler
            http://symfony.com/schema/dic/webprofiler/webprofiler-1.0.xsd">
        -->

        <webprofiler:config
            intercept-redirects="true"
        />

    .. code-block:: php
       :linenos:

        // app/config/config_dev.php
        $container->loadFromExtension('web_profiler', array(
            'intercept_redirects' => 'true',
        ));

.. tip::

    Ewentualnie można otworzyć profiler po przekierowaniu i odszukać przez
    ścięzkę URL zgłoszenia użytą przy poprzednim żądaniu (np. ``/contact/handle``).
    Funkcjonalność wyszukiwania profilera umożliwia załadowanie informacji profilera
    dla wszystkich przeszłych żądań.
