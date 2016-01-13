.. highlight:: php
   :linenothreshold: 2

.. index::
   single: bezpieczeństwo; zabezpieczanie usług
   single: bezpieczeństwo; zabezpieczanie metod
   single: metody; zabezpieczanie autoryzacyjne
   single: usługi; zabezpieczanie autoryzacyjne

Jak zabezpieczyć usługę lub metodę w aplikacji
==============================================

W rozdziale "Bezpieczeństwo" podręcznika, mogliśmy zobaczyć, jak
:ref:`zabezpieczyć kontroler <book-security-securing-controller>` przez żądanie
usługi ``security.authorization_checker`` z kontenera usług i sprawdzanie
roli bieżącego użytkownika::

    // ...
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;

    public function helloAction($name)
    {
        $this->denyAccessUnlessGranted('ROLE_ADMIN');

        // ...
    }

Można również zabezpieczyć *każdą* usługę, wstrzykując do niej usługę
``security.authorization_checker``. W celu zapoznania sie z ogólnym wprowadzeniem
w tematyke wstryzkiwaniazależności do usług, proszę zapoznać się z rozdzałem
:doc:`/book/service_container` podręcznika. Dla przykładu załóżmy, że mamy klasę
``NewsletterManager``, która wysyła wiadomości email oraz że, chcemy ograniczyć
używanie tej klasy tylko do grupy użytkowników, , którzy mają rolę
``ROLE_NEWSLETTER_ADMIN``. Klasa ta wygląda następująco, przed dodaniem zabezpieczenia::

    // src/AppBundle/Newsletter/NewsletterManager.php
    namespace AppBundle\Newsletter;

    class NewsletterManager
    {
        public function sendNewsletter()
        {
            // ... where you actually do the work
        }

        // ...
    }

Naszym celem jest sprawdzenie roli użytkownika podczas wywoływania metody
``sendNewsletter()``.
Pierwszym krokiem w tym kierunku jest wstrzyknięcie do obiektu usługi
``security.authorization_checker``. Ponieważ koniecznie trzeba wykonywać
sprawdzania zabezczenia, jest to idealny kandydat do wstrzykiwania konstruktora,
który gwarantuje, że obiekt sprawdzania autoryzacji będzie dostępny wewnatrz
klasy ``NewsletterManager``::

    // src/AppBundle/Newsletter/NewsletterManager.php

    // ...
    use Symfony\Component\Security\Core\Authorization\AuthorizationCheckerInterface;

    class NewsletterManager
    {
        protected $authorizationChecker;

        public function __construct(AuthorizationCheckerInterface $authorizationChecker)
        {
            $this->authorizationChecker = $authorizationChecker;
        }

        // ...
    }

Następnie można wstrzyknąć usługę ``security.authorization_checker``w konfiguracji
usługi ``newsletter_manager``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/services.yml
        services:
            newsletter_manager:
                class:     "AppBundle\Newsletter\NewsletterManager"
                arguments: ["@security.authorization_checker"]

    .. code-block:: xml
       :linenos:

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="newsletter_manager" class="AppBundle\Newsletter\NewsletterManager">
                    <argument type="service" id="security.authorization_checker"/>
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $container->setDefinition('newsletter_manager', new Definition(
            'AppBundle\Newsletter\NewsletterManager',
            array(new Reference('security.authorization_checker'))
        ));

Wstrzyknieta usługa może być następnie uzyta do wykonania sprawdzania zabezpieczenia
podczas wywoływania metody ``sendNewsletter()``::

    namespace AppBundle\Newsletter;

    use Symfony\Component\Security\Core\Authorization\AuthorizationCheckerInterface;
    use Symfony\Component\Security\Core\Exception\AccessDeniedException;
    // ...

    class NewsletterManager
    {
        protected $authorizationChecker;

        public function __construct(AuthorizationCheckerInterface $authorizationChecker)
        {
            $this->authorizationChecker = $authorizationChecker;
        }

        public function sendNewsletter()
        {
            if (false === $this->authorizationChecker->isGranted('ROLE_NEWSLETTER_ADMIN')) {
                throw new AccessDeniedException();
            }

            // ...
        }

        // ...
    }

Jeśli bieżący użytkownik nie ma roli ``ROLE_NEWSLETTER_ADMIN``, zostanie poproszony
o zalogowanie się.

Zabezpieczanie metod przy użyciu adnotacji
------------------------------------------

Metodę można również zabezpieczać adnotacjami, wykorzytując pakiet `JMSSecurityExtraBundle`_.
Pakiet ten nie jest dołączony do Symfony Standard Distribution, ale można go
zainstalować samemu.

W celu udostępnienia funkcjonalności adnotacji, trzeba :ref:`oznaczyć <book-service-container-tags>`
zabezpieczaną usługę tagiem ``security.secure_service`` (można również automatycznie
udostępnić tą funkcjonalność dla wszystkich usług, co jest wyjaśnione w 
:ref:`notatce bocznej <securing-services-annotations-sidebar>`, umieszczonej poniżej) :

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/services.yml
        services:
            newsletter_manager:
                class: AppBundle\Newsletter\NewsletterManager
                tags:
                    -  { name: security.secure_service }

    .. code-block:: xml
       :linenos:

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="newsletter_manager" class="AppBundle\Newsletter\NewsletterManager">
                    <tag name="security.secure_service" />
                </service>
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;
        use Symfony\Component\DependencyInjection\Reference;

        $definition = new Definition(
            'AppBundle\Newsletter\NewsletterManager',
            // ...
        ));
        $definition->addTag('security.secure_service');
        $container->setDefinition('newsletter_manager', $definition);

Można osiągnąć te same rezultaty, używając adnotacji::

    namespace AppBundle\Newsletter;

    use JMS\SecurityExtraBundle\Annotation\Secure;
    // ...

    class NewsletterManager
    {

        /**
         * @Secure(roles="ROLE_NEWSLETTER_ADMIN")
         */
        public function sendNewsletter()
        {
            // ...
        }

        // ...
    }

.. note::

    Adnotacje działają, ponieważ klasa proxy została utworzona dla naszej klasy,
    która wykonuje sprawdzanie zabezpieczenia. Oznacza to, że o ile można użyć
    adnotacji na publicznych i chronionych metodach, to nie mozna ich użyć na
    prywatnych lub finalnych metodach.

Pakiet JMSSecurityExtraBundle pozwala również zabezpieczyć parametry i zwracać
wartości metod. W celu uzyskania więcej informacji, proszę przeczytać dokumentację
`JMSSecurityExtraBundle`_.

.. _securing-services-annotations-sidebar:

.. sidebar:: Aktywowanie funkcjonalności adnotacji dla wszystkich usług

    Gdy zabezpiecza się metodę usługi (jak pokazano to powyżej), można otagować
    z osobna każdą usługę, albo aktywować tą funkcjonalność dla wszystkich
    usług jednocześnie. Dla wykonania tego, trzeba skonfigurować opcję
    ``secure_all_services`` na ``true``:

    .. configuration-block::

        .. code-block:: yaml
           :linenos:

            # app/config/services.yml
            jms_security_extra:
                # ...
                secure_all_services: true

        .. code-block:: xml
           :linenos:

            <!-- app/config/services.xml -->
            <?xml version="1.0" ?>
            <container xmlns="http://symfony.com/schema/dic/services"
                xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xmlns:jms-security-extra="http://example.org/schema/dic/jms_security_extra"
                xsi:schemaLocation="http://symfony.com/schema/dic/services
                    http://symfony.com/schema/dic/services/services-1.0.xsd">

                <!-- ... -->
                <jms-security-extra:config secure-all-services="true" />
            </container>

        .. code-block:: php
           :linenos:

            // app/config/services.php
            $container->loadFromExtension('jms_security_extra', array(
                // ...
                'secure_all_services' => true,
            ));

    Wadą tej techniki jest to, że po aktywowaniu, ładowanie pierwszej strony
    może być bardzo wolne, w zależności od ilości zdefiniowanych usług.

.. _`JMSSecurityExtraBundle`: https://github.com/schmittjoh/JMSSecurityExtraBundle
