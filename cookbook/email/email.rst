.. index::
   wiadomości email, poczta elektroniczna

Jak wysyłać wiadomości email
============================

Wysyłanie wiadomości email jest klasycznym zadaniem każdej aplikacji internetowej
i jako takie, ma swoja specyfikę i potencjalne pułapki. Zamiast "odkryawć koło",
mamy do dyspozycji gotowe rozwiązanie, jakim jest pakiet SwiftmailerBundle,
wykorzytujący bibliotekę `Swift Mailer`_. Pakiet ten wchodzi w skład Symfony
Standard Edition.

.. _swift-mailer-configuration:

Konfiguracja
------------

Korzystania z Swift Mailer jest możliwe po odpowiednim skonfigurowaniu serwera
pocztowego.

.. tip::

    Stosowanie własnego serwera pocztowego, to nie jedyne rozwiązanie. Można
    skonfigurować i używać usługi hostowanej poczty, takich jak `Mandrill`_,
    `SendGrid`_, `Amazon SES`_ i inne. Trzeba tylko podać serwer SMTP,
    nazwę użytkownika i hasło (czasami specjalne klucze), które to dane mogą
    być konfigurowane w Swift Mailer.

W standardowej instalacji Symfony jest już zawarta pewna konfiguracja
``swiftmailer``:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        swiftmailer:
            transport: "%mailer_transport%"
            host:      "%mailer_host%"
            username:  "%mailer_user%"
            password:  "%mailer_password%"

    .. code-block:: xml

        <!-- app/config/config.xml -->

        <!--
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            http://symfony.com/schema/dic/swiftmailer http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd
        -->

        <swiftmailer:config
            transport="%mailer_transport%"
            host="%mailer_host%"
            username="%mailer_user%"
            password="%mailer_password%" />

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('swiftmailer', array(
            'transport'  => "%mailer_transport%",
            'host'       => "%mailer_host%",
            'username'   => "%mailer_user%",
            'password'   => "%mailer_password%",
        ));

Wartości te (np. ``%mailer_transport%``) są odczytywane z parametrów, które są
ustawiane w pliku :ref:`parameters.yml <config-parameters.yml>`. Można zmienić
te wartości w pliku *parameters.yml* lub bezpośrednio w *config.yml*.

Dostępne są następujące atrybuty:

* ``transport``         (``smtp``, ``mail``, ``sendmail`` lub ``gmail``)
* ``username``
* ``password``
* ``host``
* ``port``
* ``encryption``        (``tls`` lub ``ssl``)
* ``auth_mode``         (``plain``, ``login`` lub ``cram-md5``)
* ``spool``

  * ``type`` (jak kolejkować wiadomości, obsługiwane sa wartości ``file`` lub
    ``memory``, czytaj :doc:`/cookbook/email/spool`)
  * ``path`` (gdzie przechowywać wiadomości)
* ``delivery_address``  (jakiś adres email na który są wysyłane wiadomości ALL)
* ``disable_delivery``  (ustawiając na true wyłącza się całkowicie dostarczanie)

Wysyłanie wiadomości
--------------------

Biblioteka Swift Mailer działa poprzez tworzenie, konfigurowanie i wysyłanie
obiektów ``Swift_Message``. Komponent "mailer" jest odpowiedzialny za rzeczywistą
dostawę wiadomości i jest dostępny za pośrednictwem usługi ``mailer``.
Ogólnie rzecz biorąc wysyłanie wiadomości jest bardzo proste::

    public function indexAction($name)
    {
        $message = \Swift_Message::newInstance()
            ->setSubject('Hello Email')
            ->setFrom('send@example.com')
            ->setTo('recipient@example.com')
            ->setBody(
                $this->renderView(
                    // app/Resources/views/Emails/registration.html.twig
                    'Emails/registration.html.twig',
                    array('name' => $name)
                ),
                'text/html'
            )
            /*
             * If you also want to include a plaintext version of the message
            ->addPart(
                $this->renderView(
                    'Emails/registration.txt.twig',
                    array('name' => $name)
                ),
                'text/plain'
            )
            */
        ;
        $this->get('mailer')->send($message);

        return $this->render(...);
    }

W celu odpowiedniego rozdzielenia elementów wiadomości, ciało wiadomości email
jest umieszczane w szablonie i renderowane przez metodę ``renderView()``. Szablon
``registration.html.twig`` może wyglądać podobnie do tego:

.. code-block:: html+jinja

    {# app/Resources/views/Emails/registration.html.twig #}
    <h3>You did it! You registered!</h3>

    {# example, assuming you have a route named "login" #}
    To login, go to: <a href="{{ url('login') }}">...</a>.

    Thanks!

    {# Makes an absolute URL to the /images/logo.png file #}
    <img src="{{ absolute_url(asset('images/logo.png')) }}"

.. versionadded:: 2.7
    W Symfony 2.7 został wprowadzona funkcja ``absolute_url()``. Wcześniej trzeba
    było stosować funkcje ``asset()`` z argumentem umożliwiającym zwracanie
    bezwzględnego adresu URL.

Obiekt ``$message`` ma wiele więcej opcji, takich jak dołączanie załączników,
dodawanie zawartości HTML i więcej. Na szczęście Swift Mailer omówiony jest
szczegółowo w swojej dokumentacji `Creating Messages`_.

.. tip::

    W "Receptariuszu" Symfony dostępnych jest kilka artykułów omawiajacych
    wysyłanie wiadomosci email:

    * :doc:`gmail`
    * :doc:`dev_environment`
    * :doc:`spool`

.. _`Swift Mailer`: http://swiftmailer.org/
.. _`Creating Messages`: http://swiftmailer.org/docs/messages.html
.. _`Mandrill`: https://mandrill.com/
.. _`SendGrid`: https://sendgrid.com/
.. _`Amazon SES`: http://aws.amazon.com/ses/
