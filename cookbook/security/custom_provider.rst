.. highlight:: php
   :linenothreshold: 2

.. index::
   single: bezpieczeństwo; dostawca użytkowników

Jak utworzyć własnego dostawcę użytkowników
===========================================

Część standardowego procesu uwierzytelniania Symfony zależy od "dostawców użytkowników".
Po zgłoszeniu przez użytkownika swojego loginu i hasła, warstwa uwierzytelniania
prosi skonfigurowanego dostawcę użytkowników, aby ten zwrócił obiekt użytkownika
dla podanej nazwy użytkownika. Symfony następnie sprawdza, czy właściwe jest hasło
dla tego użytkownika i generuje token bezpieczeństwa, tak wiec użytkownik zostaje
uwierzytlniony w bieżącej sesji.
W wersji dystrybucyjnej, Symfony posiada dostawcę użytkowników "z pamięci"
i "z encji".
W tym wpisie omawia się, jak można utworzyć własnego dostawcę, który może być
przydatny, jeśli użytkownicy są dostęþni poprzez własną bazę danych, plik lub
usługe sieciową, tak jak pokazano to w omawianym przykładzie.

Utworzenie klasy User
---------------------

Po pierwsze, niezależnie od tego, skąd dostarczane są dane użytkownika, musi się
utworzyć klasę ``User`` reprezentujacą dane użytkownika. Klasa ``User`` może
wygladac tak jak się chce i zawierać jakiekolwiek dane. Jedynym warunkiem jest to,
aby implementowała interfejs
:class:`Symfony\\Component\\Security\\Core\\User\\UserInterface`.
Interfejs ten dostarcza definicje następujacych metod, które trzeba zdefiniować
w klasie implementujacej:

* :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getRoles`,
* :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getPassword`,
* :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getSalt`,
* :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getUsername`,
* :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::eraseCredentials`.

Może być również pożyteczne zaimplementowanie interfejsu 
:class:`Symfony\\Component\\Security\\Core\\User\\EquatableInterface`,
który definiuje metodę do sprawdzania, czy dany użytkownik, to bieżący użytkownik.
Interfejs ten wymaga metody 
:method:`Symfony\\Component\\Security\\Core\\User\\EquatableInterface::isEqualTo`.

Oto jak będzie wyglądała nasza klasa ``WebserviceUser``::

    // src/AppBundle/Security/User/WebserviceUser.php
    namespace AppBundle\Security\User;

    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\User\EquatableInterface;

    class WebserviceUser implements UserInterface, EquatableInterface
    {
        private $username;
        private $password;
        private $salt;
        private $roles;

        public function __construct($username, $password, $salt, array $roles)
        {
            $this->username = $username;
            $this->password = $password;
            $this->salt = $salt;
            $this->roles = $roles;
        }

        public function getRoles()
        {
            return $this->roles;
        }

        public function getPassword()
        {
            return $this->password;
        }

        public function getSalt()
        {
            return $this->salt;
        }

        public function getUsername()
        {
            return $this->username;
        }

        public function eraseCredentials()
        {
        }

        public function isEqualTo(UserInterface $user)
        {
            if (!$user instanceof WebserviceUser) {
                return false;
            }

            if ($this->password !== $user->getPassword()) {
                return false;
            }

            if ($this->salt !== $user->getSalt()) {
                return false;
            }

            if ($this->username !== $user->getUsername()) {
                return false;
            }

            return true;
        }
    }

Jeśli zachodzi potrzeba uwzględnienia więcej informacji o użytkownikach, jak
na przykład "first name", to można dodać pole ``firstName`` do przechowywania
tych danych.

Utworzenie dostawcy użytkowników
--------------------------------

Mamy już zdefiniowaną klasę ``User``, możemy więc utworzyć dostawcę użytkownika,
który będzie pobierać informacje o użytkowniku z jakiejś usługi internetowej,
tworzyć obiekt ``WebserviceUser`` i wypałniać go danymi.

Dostawca użytkowników, to zwykła klasa PHP, która implementuje interfejs
:class:`Symfony\\Component\\Security\\Core\\User\\UserProviderInterface`,
wymagający zdefiniowania w tej klasie implementującej trzech metod:
``loadUserByUsername($username)``, ``refreshUser(UserInterface $user)``
i ``supportsClass($class)``. Proszę zapoznać sie z kodem
:class:`Symfony\\Component\\Security\\Core\\User\\UserProviderInterface`.

Oto przykład, jak może wyglądać dostawca użytkowników::

    // src/AppBundle/Security/User/WebserviceUserProvider.php
    namespace AppBundle\Security\User;

    use Symfony\Component\Security\Core\User\UserProviderInterface;
    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\Exception\UsernameNotFoundException;
    use Symfony\Component\Security\Core\Exception\UnsupportedUserException;

    class WebserviceUserProvider implements UserProviderInterface
    {
        public function loadUserByUsername($username)
        {
            // make a call to your webservice here
            $userData = ...
            // pretend it returns an array on success, false if there is no user

            if ($userData) {
                $password = '...';

                // ...

                return new WebserviceUser($username, $password, $salt, $roles);
            }

            throw new UsernameNotFoundException(
                sprintf('Username "%s" does not exist.', $username)
            );
        }

        public function refreshUser(UserInterface $user)
        {
            if (!$user instanceof WebserviceUser) {
                throw new UnsupportedUserException(
                    sprintf('Instances of "%s" are not supported.', get_class($user))
                );
            }

            return $this->loadUserByUsername($user->getUsername());
        }

        public function supportsClass($class)
        {
            return $class === 'AppBundle\Security\User\WebserviceUser';
        }
    }

Utworzenie usługi dla dostawcy użytkowników
-------------------------------------------

Teraz spowodujemy, że dostawca użytkowników będzie dostępny jako usługa:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/services.yml
        services:
            app.webservice_user_provider:
                class: AppBundle\Security\User\WebserviceUserProvider

    .. code-block:: xml
       :linenos:

        <!-- app/config/services.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <services>
                <service id="app.webservice_user_provider"
                    class="AppBundle\Security\User\WebserviceUserProvider"
                />
            </services>
        </container>

    .. code-block:: php
       :linenos:

        // app/config/services.php
        use Symfony\Component\DependencyInjection\Definition;

        $container->setDefinition(
            'app.webservice_user_provider',
            new Definition('AppBundle\Security\User\WebserviceUserProvider')
        );

.. tip::

    Prawdziwa implementacja dostawcy użytkowników będzie mieć przypuszczalnie
    kilka zależności lub opcji konfiguracyjnych lub innych usług. Dodawaj je,
    jako argumenty definicji usługi.

.. note::

    Upewnij się, że został zaimportowany plik usług. Szczegóły w atykule
    :ref:`service-container-imports-directive`.

Zmodyfikowanie ``security.yml``
-------------------------------

Informacja o wszystkich dostawcach uzytkowników powinna się znajdować w konfiguracji
bezpieczeństwa. Dodajmy dostawcę uzytkowników do listy dostawców w sekcji "security".
Wybieramy nazwę dostawcy użytkowników (np. "webservice") i wskazujemy ``id`` usługi.

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            providers:
                webservice:
                    id: app.webservice_user_provider

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

                <provider name="webservice" id="app.webservice_user_provider" />
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'providers' => array(
                'webservice' => array(
                    'id' => 'app.webservice_user_provider',
                ),
            ),
        ));

Symfony musi również mieć informację o sposobie kodowania haseł, które są wprowadzane
przez użytkowników witryny, np. przez wypełnienie formularza logowania. Można
to zrobić dodając odpowiedni wpis do sekcji "encoders" w konfiguracji bezpieczeństwa:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...

            encoders:
                AppBundle\Security\User\WebserviceUser: bcrypt

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

                <encoder class="AppBundle\Security\User\WebserviceUser"
                    algorithm="bcrypt" />
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'encoders' => array(
                'AppBundle\Security\User\WebserviceUser' => 'bcrypt',
            ),
            // ...
        ));

Wprowadzona tutaj wartość powinna odpowiadać sposobie kodowania  haseł, który
został pierwotnie użyty do zakodowania hasła przy tworzeniu użytkownika. Gdy
użytkownik wysyła swoje hasło, jest one kodowane przy użyciu tego algorytmu
i jest ono porównywane z zakodowanym hasłem zwracanym przez metodę ``getPassword()``.

.. sidebar:: Specyfika metod kodowania haseł

    Symfony używa wskazanej metody do połączenia "soli" i kodowanego hasła, przed
    porównaniem go z zakodowanego hasła. Jeśli ``getSalt()`` nie zwraca nic (brak soli),
    to wprowadzone hasło jest po prostu kodowane przy użyciu algorytmu wskazanego
    w ``security.yml``. Jeśli sól jest określona, to tworzona jest następująca
    wartość i następuje jej wymieszanie według wskazanego algorytmu::

        $password.'{'.$salt.'}'

    Jeśli zewnetrzni użytkownicy mają swoje hasła solone inną metodą, to trzeba 
    dołożyć trochę więcej pracy, aby Symfony poprawnie zakodowała takie hasło.
    Leży to poza zakresem tego artykułu, ale można to rozwiązać, dołączając
    podklasę ``MessageDigestPasswordEncoder`` i nadpisujac metodę ``mergePasswordAndSalt``.

    Można dodatkowo skonfigurować szczegóły algorytmu używanego do mieszania haseł.
    W tym przykładzie, aplikacja ustawia jawnie koszt mieszania bcrypt:

    .. configuration-block::

        .. code-block:: yaml
           :linenos:

            # app/config/security.yml
            security:
                # ...

                encoders:
                    AppBundle\Security\User\WebserviceUser:
                        algorithm: bcrypt
                        cost: 12

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

                    <encoder class="AppBundle\Security\User\WebserviceUser"
                        algorithm="bcrypt"
                        cost="12" />
                </config>
            </srv:container>

        .. code-block:: php
           :linenos:

            // app/config/security.php
            $container->loadFromExtension('security', array(
                // ...

                'encoders' => array(
                    'AppBundle\Security\User\WebserviceUser' => array(
                        'algorithm' => 'bcrypt',
                        'cost' => 12,
                    ),
                ),
            ));

.. _MessageDigestPasswordEncoder: https://github.com/symfony/symfony/blob/master/src/Symfony/Component/Security/Core/Encoder/MessageDigestPasswordEncoder.php
