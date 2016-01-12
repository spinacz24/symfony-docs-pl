.. index::
   Gmail
   single: wiadomości email; Gmail
   single: poczta elektroniczna; Gmail

Jak wykorzystywać Gmail do wysyłania wiadomości email
=====================================================

W trakcie prac programistycznych, można wykorzystać Gmail zamiast zwykłego serwera
SMTP, co może być łatwiejsze i praktyczniejsze. SwiftmailerBundle radzi sobie z
tym naprawdę łatwo.

.. tip::

    Sugerujemy, aby zamiast korzystania z osobistego konta Gmail utworzyć sobie
    specjalne konto do tych celów.

W programistycznym pliku konfiguracyjnym trzeba zmienić ustawienie ``transport``
na ``gmail`` i ustawić ``username`` i ``password`` zgodnie z poświadczeniem Google:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config_dev.yml
        swiftmailer:
            transport: gmail
            username:  your_gmail_username
            password:  your_gmail_password

    .. code-block:: xml
       :linenos:

        <!-- app/config/config_dev.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:swiftmailer="http://symfony.com/schema/dic/swiftmailer"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/swiftmailer
                http://symfony.com/schema/dic/swiftmailer/swiftmailer-1.0.xsd">

            <!-- ... -->
            <swiftmailer:config
                transport="gmail"
                username="your_gmail_username"
                password="your_gmail_password"
            />
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config_dev.php
        $container->loadFromExtension('swiftmailer', array(
            'transport' => 'gmail',
            'username'  => 'your_gmail_username',
            'password'  => 'your_gmail_password',
        ));

Gotowe!

.. tip::

    Jeśli używa się Symfony Standard Edition, trzeba skonfigurować parametry w
    pliku ``parameters.yml``:

    .. code-block:: yaml
       :linenos:

        # app/config/parameters.yml
        parameters:
            # ...
            mailer_transport: gmail
            mailer_host:      ~
            mailer_user:      your_gmail_username
            mailer_password:  your_gmail_password

.. note::

    Wartość ``gmail`` transportu jest po prostu skrótem, który wykorzystuje
    transport ``smtp`` i ustawia opcje ``encryption``, ``auth_mode`` i ``host``
    na pracę z Gmail.

.. note::

    W zależności od ustawień konta Gmail, mogą pojawić się w aplikacji błędy
    uwierzytelniania. Jeśli na koncie Gmail stosuje się dwustopniową weryfikację,
    trzeba `wygenerować hasło App`_ w celu użycia w parametrze ``mailer_password``.
    Trzeba się też upewnić, że ustawiona jest opcja
    `zezwolenia na dostęp do konta Gmail mniej bezpiecznych aplikacji`_.

.. _`wygenerować hasło App`: https://support.google.com/accounts/answer/185833
.. _`zezwolenia na dostęp do konta Gmail mniej bezpiecznych aplikacji`: https://support.google.com/accounts/answer/6010255
