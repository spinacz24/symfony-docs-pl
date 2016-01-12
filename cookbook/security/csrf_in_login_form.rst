.. index::
    single: bezpieczeństwo; formularz logowania; ataki CSRF

Zabezpieczanie formularza logowania przed atakami CSRF
======================================================

W przypadki stosowania formularza logowania, trzeba mieć pewność, że jest on
zabezpieczony przed atakami CSRF (`Cross-site request forgery`_). Komponent
Security ma już wbudowaną obsługę dla ochrony CSRF. W tym artykule dowiesz się,
jak używać to w swoim formularzu logowania.

.. note::

    Ataki CSRF na formularz logowania są nieco mniej znane. Rzecz omówiona jest
    jest w anglojezycznym tekscie `Forging Login Requests`_.

Konfiguracja ochrony CSRF
-------------------------

Najpierw, trzeba skonfigurować komponent Security tak, aby zastosować ochronę
CSRF.
Komponent Security wymaga dostawcy tokenu CSRF. Można ja ustawić tak, aby wykorzystywany
był dostawca domyślny, dostępny w komponencie Security:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                secured_area:
                    # ...
                    form_login:
                        # ...
                        csrf_provider: security.csrf.token_manager

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="secured_area">
                    <!-- ... -->
                    <form-login csrf-provider="security.csrf.token_manager" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'secured_area' => array(
                    // ...
                    'form_login' => array(
                        // ...
                        'csrf_provider' => 'security.csrf.token_manager',
                    ),
                ),
            ),
        ));

Komponent Security można skonfigurować bardziej, ale dla zabezpieczenia formularza
logowania przed atakami CSRF, to całkowicie wystarczy.

Renderowanie pola CSRF formularza
---------------------------------

Teraz, komponent Security będzie sprawdzał token CSRF token, ale wymaga to, dodania
w wformularzu ukrytego pola, zawierajacego ten token. Domyślnie, pole to ma
nazwę ``_csrf_token``. To ukryte pole musi zawierać token CSRF, który można
wygenerować używając funkcji ``csrf_token``. Funkcja ta wymaga podania wartości 
identyfikatora tokenu, który trzeba ustawić na ``authenticate``, gdy używa się
formularza logowania:

.. configuration-block::

    .. code-block:: html+jinja
       :linenos:

        {# src/Acme/SecurityBundle/Resources/views/Security/login.html.twig #}

        {# ... #}
        <form action="{{ path('login_check') }}" method="post">
            {# ... the login fields #}

            <input type="hidden" name="_csrf_token"
                value="{{ csrf_token('authenticate') }}"
            >

            <button type="submit">login</button>
        </form>

    .. code-block:: html+php
       :linenos:

        <!-- src/Acme/SecurityBundle/Resources/views/Security/login.html.php -->

        <!-- ... -->
        <form action="<?php echo $view['router']->generate('login_check') ?>" method="post">
            <!-- ... the login fields -->

            <input type="hidden" name="_csrf_token"
                value="<?php echo $view['form']->csrfToken('authenticate') ?>"
            >

            <button type="submit">login</button>
        </form>

Teraz, formularz logowania jest już zabezpieczony przed atakami CSRF.

.. tip::

    Można zmienić nazwę pola przechowującego token przez zmianę ustawienia
    ``csrf_parameter`` oraz zmianę identyfikatora tokenu, ustawiając w konfiguracji
    opcje ``intention``:

    .. configuration-block::

        .. code-block:: yaml
           :linenos:

            # app/config/security.yml
            security:
                # ...

                firewalls:
                    secured_area:
                        # ...
                        form_login:
                            # ...
                            csrf_parameter: _csrf_security_token
                            intention: a_private_string

        .. code-block:: xml
           :linenos:

            <!-- app/config/security.xml -->
            <?xml version="1.0" encoding="UTF-8" ?>
            <srv:container xmlns="http://symfony.com/schema/dic/security"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:srv="http://symfony.com/schema/dic/services"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd">

                <config>
                    <!-- ... -->

                    <firewall name="secured_area">
                        <!-- ... -->
                        <form-login csrf-parameter="_csrf_security_token"
                            intention="a_private_string"
                        />
                    </firewall>
                </config>
            </srv:container>

        .. code-block:: php
           :linenos:

            // app/config/security.php
            $container->loadFromExtension('security', array(
                // ...

                'firewalls' => array(
                    'secured_area' => array(
                        // ...
                        'form_login' => array(
                            // ...
                            'csrf_parameter' => '_csrf_security_token',
                            'intention'      => 'a_private_string',
                        ),
                    ),
                ),
            ));

.. _`Cross-site request forgery`: https://pl.wikipedia.org/wiki/Cross-site_request_forgery
.. _`Forging Login Requests`: https://en.wikipedia.org/wiki/Cross-site_request_forgery#Forging_login_requests
