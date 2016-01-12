.. index::
   single: wiadomości email; wykorzystywanie chmury
   single: poczta elektroniczna; wykorzystywanie chmury

Jak wykorzystywać chmurę do wysyłania wiadomości email
======================================================

Wymagania dotyczące wysyłania wiadomości email z poziomu systemu produkcyjnego
są inne niż w konfiguracji środowiska programistycznego, ponieważ nie będzie się
w nim ograniczało ilości wiadomości, rat czy adresów nadawców.
Dlatego :doc:`uzywanie Gmail </cookbook/email/gmail>` lub innej podobnej usługi
nie jest rozwiązaniem. Jeśli skonfigurowanie i utrzymanie własnego serwera
jest za bardzo kłopotliwe, to jest proste rozwiązanie: wykorzystanie chmury
do wysyłania poczty.

Opisany tu sposób pokazuje jak można prosto zintegrować z Symfony usługę
`Amazon's Simple Email Service (SES)`_ .

.. note::

    Tą technikę można wykorzystać przy innych usługach pocztowych, ponieważ cały
    problem sprowadza się do odpowiedniego skonfigurowania punktu końcowego
    SMTP w Swift Mailer.

W konfiguracji Symfony zmień ustawienia Swift Mailer w opcjach ``transport``,
``host``, ``port`` i ``encryption``, zgodnie z informacją zawartą na `konsoli SES`_.
W konsoli SES utwórz swoje indywidualne poświadczenie SMTP i zakończ konfigurację
podając wartości ``username`` i ``password``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/config.yml
        swiftmailer:
            transport:  smtp
            host:       email-smtp.us-east-1.amazonaws.com
            port:       465 # different ports are available, see SES console
            encryption: tls # TLS encryption is required
            username:   AWS_ACCESS_KEY  # to be created in the SES console
            password:   AWS_SECRET_KEY  # to be created in the SES console

    .. code-block:: xml
       :linenos:

        <!-- app/config/config.xml -->
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
                transport="smtp"
                host="email-smtp.us-east-1.amazonaws.com"
                port="465"
                encryption="tls"
                username="AWS_ACCESS_KEY"
                password="AWS_SECRET_KEY"
            />
        </container>

    .. code-block:: php
       :linenos:

        // app/config/config.php
        $container->loadFromExtension('swiftmailer', array(
            'transport'  => 'smtp',
            'host'       => 'email-smtp.us-east-1.amazonaws.com',
            'port'       => 465,
            'encryption' => 'tls',
            'username'   => 'AWS_ACCESS_KEY',
            'password'   => 'AWS_SECRET_KEY',
        ));

Klucze ``port`` i ``encryption`` nie znajdują się w domyślnej konfiguracji
Symfony Standard Edition, ale można je dodać, gdy jest taka potrzeba.

Jak sie to zrobi, można zacząć wysyłanie wiadomości email przez chmurę!

.. tip::

    Jeśli używa się Symfony Standard Edition, konfiguruj parametry w pliku
    ``parameters.yml`` i wykorzystuj je w swoich plikach konfiguracyjnych.
    Umożliwia to różną konfigurację Swift Mailer dla każdej instalacji aplikacji.
    Na przykład, używanie Gmail podczas prac programistycznych i chmury w środowisku
    produkcyjnym.

    .. code-block:: yaml
       :linenos:

        # app/config/parameters.yml
        parameters:
            # ...
            mailer_transport:  smtp
            mailer_host:       email-smtp.us-east-1.amazonaws.com
            mailer_port:       465 # different ports are available, see SES console
            mailer_encryption: tls # TLS encryption is required
            mailer_user:       AWS_ACCESS_KEY # to be created in the SES console
            mailer_password:   AWS_SECRET_KEY # to be created in the SES console

.. note::

    Jeśli ma się zamiar używać Amazon SES, trzeba mieć na uwadze nastęþujące rzeczy:

    * Trzeba zarejestrować się na `Amazon Web Services (AWS)`_;

    * Każdy adres uzywany w nagłówku ``From`` lub ``Return-Path`` (adres zwrotny)
      musi być potwierdzony przez właściciela. Można również potwierdzić całą
      domenę;

    * Początkowo ma się narzucone ograniczenia trybu piaskownicy. Trzeba wystąpić
      o dostęp produkcyjnym, zanim rozpocznie się wysyłanie wiadomości do stałych
      odpiorców;

    * SES może podlegać opłacie.

.. _`Amazon's Simple Email Service (SES)`: http://aws.amazon.com/ses
.. _`konsoli SES`: https://console.aws.amazon.com/ses
.. _`Amazon Web Services (AWS)`: http://aws.amazon.com
