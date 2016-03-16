.. index::
   single: bezpieczeństwo; logowanie formularzowe
   single: logowanie formularzowe; dostosowywanie

Jak dostosować logowanie formularzowe
=====================================

Stosowanie :doc:`logowania formularzowego </cookbook/security/form_login_setup>`
jest powszechnym i elastycznym sposobem obsługi uwierzytelniania w Symfony.
Dostosować można prawie każdy aspekt logowania formularzowego. Pełna, domyślna
konfiguracja została pokazana w następnym rozdziale.

Informacje o konfiguracji logowania formularzowego
--------------------------------------------------

Pełne informacje o konfiguracji logowania formularzowego zostały podane w artykule
:doc:`/reference/configuration/security`. Niektóre, bardziej interesujace rzeczy
zostały wyjaśnione poniżej.

Przekierowanie po udanym logowaniu
----------------------------------

Można zmienić ustawienie wskazujące, gdzie ma zostać przekierowany użytkownik
po pomyślnym zalogowaniu, używając różnych opcji konfiguracyjnych. Domyślnie
formularz logowania jest przekierowywany do adresu URL żądanego przez użytkownika
(czyli żądanego zasobu, który wyzwala formularz logowania).
Na przykład, jeśli użytkownik żąda ``http://www.example.com/admin/post/18/edit``,
to po pomyślnym zalogowaniu, użytkownik zostanie przekierowany z formularza
logowania z powrotem do ``http://www.example.com/admin/post/18/edit``.
Odbywa się to przez zapisanie żądanego adresu URL w sesji.
Jeśli żaden adres URL nie znajduje sie w sesji (być może użytkownik przeszedł
bezpośrednio do strony logowania), to użytkownik jest przekierowywany do strony
domyślnej, czyli na ścieżkę ``/`` (stronę poczatkową). Można zmienić to zachowanie
na kilka sposobów.

.. note::

    Jak wspomniano, użytkownik domyślnie jest przekierowywany z powrotem do
    pierwotnie żądanej strony. Czasem stwarza to problemy, podobnie jak dla żądań
    zwrotnych Ajax "pozornie" będących ostatnio odwiedzanym adresem URL, przez
    co użytkownik będzie tam przekierowywany. Więcej informacji o kontrolowaniu
    tego zachowania można znaleźć w artykule :doc:`/cookbook/security/target_path`.

Zmienianie domyślnej strony
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Po pierwsze, może zostać ustawiona strona domyślna (czyli strona, na którą są
przekierowywaniu użytkonicy, po pomyślnym zalogowaniu, jeśli w sesji brak jest
zapisu o ścieżce żądanego zasobu). W celu ustawienia tego na
trasę ``default_security_target``, trzeba użyć następującej konfiguracji:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                main:
                    form_login:
                        # ...
                        default_target_path: default_security_target

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <form-login default-target-path="default_security_target" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'main' => array(
                    // ...

                    'form_login' => array(
                        // ...
                        'default_target_path' => 'default_security_target',
                    ),
                ),
            ),
        ));

Teraz, gdy żaden adres nie zostanie zapisany w sesji, użytkownicy bedą wysyłani
na trasę ``default_security_target``.

Zawsze stosuj przekierowywanie do domyślnej strony
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Można zrobić to tak, że użytkownicy zawsze będą przekierowywani do domyślnej
strony, niezależnie od żądanego przez użytkownika adresu URL, przez ustawienie
opcji ``always_use_default_target_path`` na ``true``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                main:
                    form_login:
                        # ...
                        always_use_default_target_path: true

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->
                    <form-login always-use-default-target-path="true" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'main' => array(
                    // ...

                    'form_login' => array(
                        // ...
                        'always_use_default_target_path' => true,
                    ),
                ),
            ),
        ));

Korzystanie z odniesień URL
~~~~~~~~~~~~~~~~~~~~~~~~~~~

W przypadku, gdy żaden adres URL nie został zapisany w sesji, można chcieć
stosować odniesienie ``HTTP_REFERER``, ale często wyjdzie to na to samo.
Można to zrobić ustawiając opcję ``use_referer`` na ``true`` (wartość domyślna
to ``false``):

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    form_login:
                        # ...
                        use_referer: true

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->
                    <form-login use-referer="true" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'main' => array(
                    // ...
                    'form_login' => array(
                        // ...
                        'use_referer' => true,
                    ),
                ),
            ),
        ));

Kontrolowanie przekierowania URL z poziomu formularza
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Można również nadpisać cel przekierowania z poziomu samego formularza logowania,
wstawiając w nim ukryte pole o nazwie ``_target_path``. Na przykład, aby przekierować
użytkownika do adresu URL określonego prez jakąś trasę ``account``, użyj poniższy
kod:

.. configuration-block::

    .. code-block:: html+jinja
       :linenos:

        {# src/Acme/SecurityBundle/Resources/views/Security/login.html.twig #}
        {% if error %}
            <div>{{ error.message }}</div>
        {% endif %}

        <form action="{{ path('login_check') }}" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username" name="_username" value="{{ last_username }}" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            <input type="hidden" name="_target_path" value="account" />

            <input type="submit" name="login" />
        </form>

    .. code-block:: html+php
       :linenos:

        <!-- src/Acme/SecurityBundle/Resources/views/Security/login.html.php -->
        <?php if ($error): ?>
            <div><?php echo $error->getMessage() ?></div>
        <?php endif ?>

        <!-- W Symfony 2.8 została wprowadzona metoda path(). Wczesniej, trzeba
             było uzywać metode generate(). -->
        <form action="<?php echo $view['router']->path('login_check') ?>" method="post">
            <label for="username">Username:</label>
            <input type="text" id="username" name="_username" value="<?php echo $last_username ?>" />

            <label for="password">Password:</label>
            <input type="password" id="password" name="_password" />

            <input type="hidden" name="_target_path" value="account" />

            <input type="submit" name="login" />
        </form>

Teraz, użytkownik zostanie przekierowany do adresu wskazanego w ukrytym polu
formularza. Wartością atrybutu może być ścieżka względna, bezwzgledny adres URL
lub nazwa trasy. Można nawet zmienić nazwę ukrytego pola formularza, zmieniając
opcję ``target_path_parameter`` na inną wartość.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    form_login:
                        target_path_parameter: redirect_url

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->
                    <form-login target-path-parameter="redirect_url" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'main' => array(
                    // ...
                    'form_login' => array(
                        'target_path_parameter' => 'redirect_url',
                    ),
                ),
            ),
        ));

Przekierowanie przy nieudanym logowaniu
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Oprócz przekierowania użytkownika po udanym logowaniu, można też ustawić adres
URL na który będzie przekierowywany uzytkonik po nieudanym logowaniu
(np. z powodu wprowadzenia błędnej nazwy użytkownika lub hasła). Domyślnie,
użytkownik jest przekierowywany z powrotem do formularza logowania.
Można ustawić to na inną trasę (np. ``login_failure``) w następujacy sposób:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            firewalls:
                main:
                    # ...
                    form_login:
                        # ...
                        failure_path: login_failure

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <firewall name="main">
                    <!-- ... -->
                    <form-login failure-path="login_failure" />
                </firewall>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'firewalls' => array(
                'main' => array(
                    // ...
                    'form_login' => array(
                        // ...
                        'failure_path' => 'login_failure',
                    ),
                ),
            ),
        ));
