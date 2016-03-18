.. index::
   single: bezpieczeństwo; komponent Security
   single: komponent Security; wprowadzenie

Komponent Security
==================

    Komponent Security dostarcza kompletny system bezpieczeństwa dla aplikacji
    internetowej. Dostarczany jest z mechanizmami podstawowego uwierzytelniania
    HTTP lub uwierzytelniania Digest, interaktywnego logowania formularzowego
    lub logowania z certyfikatem X.509, ale też stworzona jest możliwość implementacji
    własnych strategii uwierzytelniania.
    Ponadto, komponent zapewnia sposoby na autoryzowanie uwierzytelnionych użytkowników,
    w oparciu o ich role i zawiera zaawansowany system ACL.

Instalacja
----------

Komponent ten można zainstalować na dwa sposoby:

* :doc:`Instalacja poprzez Composer </components/using_components>` (``symfony/security`` na Packagist_);
* Wykorzystanie oficjalnego repozytorium Git (https://github.com/symfony/security).

.. include:: /components/require_autoload.rst.inc

Komponent Security jest podzielony na cztery mniejsze podkomponenty, które można
stosować oddzielnie:

``symfony/security-core``
    Dostarcza wszystkie powszechne funkcjonalności bezpieczeństwa, od uwierzytelniania
    do autoryzacji i od kodowania haseł do ładowania uzytkowników.

``symfony/security-http``
    Integruje w podkomponencie rdzenia protokół HTTP do obsługi żądań i odpowiedzi
    HTTP.

``symfony/security-csrf``
    Zapewnia ochrone przed `atakami CSRF`_.

``symfony/security-acl``
    Zapewnia precyzyjny mechanizm uprawnień oparty na listach kontroli dostępu ACL.

Rozdziały
---------

* :doc:`/components/security/firewall`
* :doc:`/components/security/authentication`
* :doc:`/components/security/authorization`
* :doc:`/components/security/secure_tools`

.. _Packagist: https://packagist.org/packages/symfony/security
.. _`atakami CSRF`: https://en.wikipedia.org/wiki/Cross-site_request_forgery
