.. highlight:: php
   :linenothreshold: 2

.. index::
   single: bezpieczeństwo; dostawca użytkowników
   single: bezpieczeństwo; dostawca z encji

Jak wykonać ładowanie użytkowników z bazy danych (dostawca z encji)
===================================================================

System bezpieczeństwa Symfony może ładować swoich użytkownikow z różnych miejsc,
takich jak baza danych, serwer LDAP, Active Directory czy OAuth. Artykuł
ten wyjaśnia, jak ładować użytkowników z bazy danych poprzez encję Doctrine.

Wprowadzenie
------------

.. tip::

    Przed rozpoczęciem, należy sprawdzić pakiet `FOSUserBundle`_. Ten zewnętrzny
    pakiet umożliwia ładowanie użytkowników z bazy danych (podobnie jak wyjaśniamy
    to tutaj) i dostarcza wbudowane trasy i kontrolery dla takich rzeczy jak
    logowanie, rejestracja i funkcja "zapomniałem hasło". Jeśli jednak potrzebujesz
    bardziej dostosować swój system użytkowników lub jeśli chcesz nauczyć sie jak
    ten system działa, to ten poradnik jest jeszcze lepszy.

Wykonanie funkcjonalności ładowania użytkowników poprzez encję Doctrine opisujemy
jako dwa etapy:

#. :ref:`Utworzenie encji User <security-crete-user-entity>`
#. :ref:`Skonfigurowanie security.yml tak, aby ładował użytkowników z tej encji <security-config-entity-provider>`

Następnie, wyjaśniamy takie rzeczy jak: 
:ref:`zakazywanie ładowania nieaktywnych użytkowników <security-advanced-user-interface>`,
:ref:`używanie własnych zapytań <authenticating-someone-with-a-custom-entity-provider>`
i :ref:`serializowanie użytkownika dla sesji <cookbook-security-serialize-equatable>`

.. _security-crete-user-entity:
.. _the-data-model:

1) Utworzenie encji User
------------------------

Dla celów tego artykułu przyjmijmy, ze mamy już encję ``User`` wewnatrz pakietu
``AppBundle`` z następującymi polami : ``id``, ``username``, ``password``,
``email`` i ``isActive``:

.. code-block:: php
   :linenos:

    // src/AppBundle/Entity/User.php
    namespace AppBundle\Entity;

    use Doctrine\ORM\Mapping as ORM;
    use Symfony\Component\Security\Core\User\UserInterface;

    /**
     * @ORM\Table(name="app_users")
     * @ORM\Entity(repositoryClass="AppBundle\Entity\UserRepository")
     */
    class User implements UserInterface, \Serializable
    {
        /**
         * @ORM\Column(type="integer")
         * @ORM\Id
         * @ORM\GeneratedValue(strategy="AUTO")
         */
        private $id;

        /**
         * @ORM\Column(type="string", length=25, unique=true)
         */
        private $username;

        /**
         * @ORM\Column(type="string", length=64)
         */
        private $password;

        /**
         * @ORM\Column(type="string", length=60, unique=true)
         */
        private $email;

        /**
         * @ORM\Column(name="is_active", type="boolean")
         */
        private $isActive;

        public function __construct()
        {
            $this->isActive = true;
            // may not be needed, see section on salt below
            // $this->salt = md5(uniqid(null, true));
        }

        public function getUsername()
        {
            return $this->username;
        }

        public function getSalt()
        {
            // you *may* need a real salt depending on your encoder
            // see section on salt below
            return null;
        }

        public function getPassword()
        {
            return $this->password;
        }

        public function getRoles()
        {
            return array('ROLE_USER');
        }

        public function eraseCredentials()
        {
        }

        /** @see \Serializable::serialize() */
        public function serialize()
        {
            return serialize(array(
                $this->id,
                $this->username,
                $this->password,
                // see section on salt below
                // $this->salt,
            ));
        }

        /** @see \Serializable::unserialize() */
        public function unserialize($serialized)
        {
            list (
                $this->id,
                $this->username,
                $this->password,
                // see section on salt below
                // $this->salt
            ) = unserialize($serialized);
        }
    }

W celu uproszczenia nie pokazaliśmy tutaj niektórych metod getter i setter.
Uruchamiając polecenie:

.. code-block:: bash

    $ php bin/console doctrine:generate:entities AppBundle/Entity/User

można :ref:`wygenerować <book-doctrine-generating-getters-and-setters>`
wszystkie metody akcesorów getter i setter.

Następnie trzeba się upewnić, że :ref:`stworzona została tabela bazy danych <book-doctrine-creating-the-database-tables-schema>`:

.. code-block:: bash

    $ php bin/console doctrine:schema:update --force

Co to jest UserInterface?
~~~~~~~~~~~~~~~~~~~~~~~~~

Jak dotąd, jest to zwykła encja, aby jednak użyć tej klasy w systemie bezpieczeństwa,
musi się zaimplementować interfejs
:class:`Symfony\\Component\\Security\\Core\\User\\UserInterface`.
W efekcie, klasa będzie miała pięć następujących metod:

* :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getRoles`
* :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getPassword`
* :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getSalt`
* :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::getUsername`
* :method:`Symfony\\Component\\Security\\Core\\User\\UserInterface::eraseCredentials`

Dla zapoznania się ze szczegółami dotyczącymi każdej z nich, proszę przeanalizować
kod :class:`Symfony\\Component\\Security\\Core\\User\\UserInterface`.

Jak działają metody serialize i unserialize?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Na koniec przetawarzania każdego żądania, obiekt User jest serializowany do sesji.
Przy kolejnym żądaniu, zostaje on odserializowany. W celu wspomożenia PHP w tym
zadaniu, trzeba zaimplementować ``Serializable``. Lecz nie trzeba serializować
wszystkiego. Wystarczy zserialiować tylko kilka pól (te pokazane powyżej plus
kilka dodatkowych, jeśli zdecyduje się na implementację interfejsu
:ref:`AdvancedUserInterface <security-advanced-user-interface>`).
Przy każdym żądaniu, w zapytaniu do bazy danych zostaje użyty  identyfikator
świeżego obiektu ``User``.

Chcesz dowiedzieć się więcej? Przeczytaj :ref:`cookbook-security-serialize-equatable`.

.. _authenticating-someone-against-a-database:
.. _security-config-entity-provider:

2) Konfigurowanie bezpieczeństwa do ładowania użytkowników z encji
------------------------------------------------------------------

Teraz, gdy już mamy encję ``User`` implementująca ``UserInterface``, wystarczy
powiadomić system bezpieczeństwa Symfony w pliku ``security.yml``.

W tym przykładzie, użytkownik będzie wprowadzał swoją nazwę i hasło wykorzystując
podstawowe uwierzytelnianie HTTP. Symfony będzie pytać encję ``User`` pasującą
do nazwy użytkownika i sprawdzać hasło (o haśle więcej za moment):

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # bin/config/security.yml
        security:
            encoders:
                AppBundle\Entity\User:
                    algorithm: bcrypt

            # ...

            providers:
                our_db_provider:
                    entity:
                        class: AppBundle:User
                        property: username
                        # if you're using multiple entity managers
                        # manager_name: customer

            firewalls:
                default:
                    pattern:    ^/
                    http_basic: ~
                    provider: our_db_provider

            # ...

    .. code-block:: xml
       :linenos:

        <!-- bin/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <encoder class="AppBundle\Entity\User" algorithm="bcrypt" />

                <!-- ... -->

                <provider name="our_db_provider">
                    <!-- if you're using multiple entity managers, add:
                         manager-name="customer" -->
                    <entity class="AppBundle:User" property="username" />
                </provider>

                <firewall name="default" pattern="^/" provider="our_db_provider">
                    <http-basic />
                </firewall>

                <!-- ... -->
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // bin/config/security.php
        $container->loadFromExtension('security', array(
            'encoders' => array(
                'AppBundle\Entity\User' => array(
                    'algorithm' => 'bcrypt',
                ),
            ),

            // ...

            'providers' => array(
                'our_db_provider' => array(
                    'entity' => array(
                        'class'    => 'AppBundle:User',
                        'property' => 'username',
                    ),
                ),
            ),
            'firewalls' => array(
                'default' => array(
                    'pattern'    => '^/',
                    'http_basic' => null,
                    'provider'   => 'our_db_provider',
                ),
            ),

            // ...
        ));

Po pierwsze, sekcja ``encoders`` informuje Symfony, że hasła znajdujące się 
w bazie danych są szyfrowane przy użyciu ``bcrypt``. Po drugie, sekcja ``providers``
konfiguruje "dostawcę użytkowników" o nazwie ``our_db_provider``, który wykorzystuje
zapytanie z encji ``AppBundle:User`` wg. własności ``username``.
Nazwa ``our_db_provider`` nie jest istotna: jest ona potrzebna tylko do dopasowania
wartości klucza ``provider`` w naszej zaporze. Jeśli nie ustawi się w zaporze
klucza ``provider``, zostanie użyty automatycznie pierwszy "dostawca użytkowników".


Utworzenie pierwszego użytkownika
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

W celu dodania użytkownikow, można zaimplementować
:doc:`formularz rejestracyjny </cookbook/doctrine/registration_form>`
lub dodać kilka `fikstur`_. Jest to po prostu zwykła encja, tak więc nie ma tu nic
trudnego, chyba że, będzie sie chciało zakodować hasło każdego użytkownika.
Lecz się  nie martw, Symfony dostarcza usługę, która zrobi to za Ciebie.
Prosze zapoznać się z rozdziałem :ref:`security-encoding-password` w celu poznania
szczegółów.

Poniżej znajduje sie wynik zapytania do tabeli ``app_users`` z bazy MySQL.
Wykazany jest użytkownik ``admin`` z hasłem ``admin`` (jest ono zakodowane).

.. code-block:: bash

    $ mysql> SELECT * FROM app_users;
    
+----+----------+--------------------------------------------------------------+-------------------+-----------+
| id | username | password                                                     | email             | is_active |
+----+----------+--------------------------------------------------------------+-------------------+-----------+
| 1  | admin    | $2a$08$jHZj/wJfcVKlIwr5AvR78euJxYK7Ku5kURNhNx.7.CSIJ3Pq6LEPC | admin@example.com | 1         |
+----+----------+--------------------------------------------------------------+-------------------+-----------+

.. sidebar:: Czy potrzeba dodawać właściwość Salt?

    Nie, jeśli używa się algorytmu ``bcrypt``. W przeciwnym razie, tak.
    Wszystkie hasła muszą zostać zmieszane z solą, lecz ``bcrypt`` robi to
    wewnętrznie. Ponieważ w tym poradniku uzywamy algorytmu ``bcrypt``, metoda
    ``getSalt()`` w ``User`` po prostu zwraca ``null`` (nie jest używana).
    Jeśli używa się innego algorytmu, należy odkomentować linie ``salt`` w encji
    ``User`` i dodać utrwalającą właściwość ``salt``.

.. _security-advanced-user-interface:

Zabronienie dostępu nieaktywnym użytkownikom (AdvancedUserInterface)
--------------------------------------------------------------------

Jeśli właściwość ``isActive`` obiektu użytkownika jest ustawiona na ``false`` (czyli
pole ``is_active`` w bazie danych ma wartość 0), użytkownik będzie mógł nadal
logować się do witryny. Można to łatwo naprawić.

W celu wykluczenia nieaktywnych użytkowników, należy zmienić klasę ``User`` tak,
aby implementowała :class:`Symfony\\Component\\Security\\Core\\User\\AdvancedUserInterface`.
Rozszerza to interfejs :class:`Symfony\\Component\\Security\\Core\\User\\UserInterface`,
tak więc potrzebny jest tylko nowy interfejs::

    // src/AppBundle/Entity/User.php

    use Symfony\Component\Security\Core\User\AdvancedUserInterface;
    // ...

    class User implements AdvancedUserInterface, \Serializable
    {
        // ...

        public function isAccountNonExpired()
        {
            return true;
        }

        public function isAccountNonLocked()
        {
            return true;
        }

        public function isCredentialsNonExpired()
        {
            return true;
        }

        public function isEnabled()
        {
            return $this->isActive;
        }

        // serialize and unserialize must be updated - see below
        public function serialize()
        {
            return serialize(array(
                // ...
                $this->isActive
            ));
        }
        public function unserialize($serialized)
        {
            list (
                // ...
                $this->isActive
            ) = unserialize($serialized);
        }
    }

Interfejs :class:`Symfony\\Component\\Security\\Core\\User\\AdvancedUserInterface`
dodaje cztery metody w celu walidowania stanu konta:

* :method:`Symfony\\Component\\Security\\Core\\User\\AdvancedUserInterface::isAccountNonExpired`
  sprawdza, czy konto użytkownika wygasło;
* :method:`Symfony\\Component\\Security\\Core\\User\\AdvancedUserInterface::isAccountNonLocked`
  sprawdza, czy użytkownik jest zablokowany;
* :method:`Symfony\\Component\\Security\\Core\\User\\AdvancedUserInterface::isCredentialsNonExpired`
  sprawdza, czy poświadczenie (hasło) użytkownika wygasło;
* :method:`Symfony\\Component\\Security\\Core\\User\\AdvancedUserInterface::isEnabled`
  sprawdza, czy użytkownik jest włączony.

Jeśli któraś z tych metod zwraca ``false``, użytkownik nie będzie się mógł zalogować.
Można zrobić tak, aby mieć utrwalanie dla wszystkich tych metod
lub tylko dla niektórych (w naszym przykładzie utrwalana jest w bazie danych tylko
właściwość ``isActive``).

Więc jaka jest różnica pomiędzy tymi metodami? Każda zwraca nieco inny komunikat
błędu (i może on być przetłumaczony).

.. note::

    Jeśli używa się ``AdvancedUserInterface``, może być potrzebne dodanie którejś
    z właściwości używanej przez te metody (jak ``isActive``) do metod ``serialize()``
    i ``unserialize()``. Jeśli tego się nie zrobi, użytkownik może nie zostać
    ppoprawnie zdeserializowany przy sesji każdego żądania.

Zrobione! Nasz system bezpieczeństwa ładowany z bazy danych jest w pełni skonfigurowany!
Następnie, trzeba zmienić na true opcję :doc:`formularza logowania </cookbook/security/form_login>`
aby przejść z podstawowego logowania HTTP na logowanie formularzowe.

.. _authenticating-someone-with-a-custom-entity-provider:

Używanie własnych zapytań dla ładowania użytkowników
----------------------------------------------------

Byłoby wspaniale, gdyby użytkownik mógł się logować podając swoją nazwę lub
swój adres email, jako że obydwa pola są unikalne w bazie danych. Niestety,
natywny dostawca encji umożliwia obsługę zapytań z wykorzystaniem tylko jednej
właściwości obiektu użytkownika.

W celu zrobienie tego wykonamy ``UserRepository`` implementując specjalną klasę
:class:`Symfony\\Bridge\\Doctrine\\Security\\User\\UserLoaderInterface`.
Interfejs ten wymaga tylko jednej metosy ``loadUserByUsername($username)``::

    // src/AppBundle/Entity/UserRepository.php
    namespace AppBundle\Entity;

    use Symfony\Bridge\Doctrine\Security\User\UserLoaderInterface;
    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\Exception\UsernameNotFoundException;
    use Doctrine\ORM\EntityRepository;

    class UserRepository extends EntityRepository implements UserLoaderInterface
    {
        public function loadUserByUsername($username)
        {
            $user = $this->createQueryBuilder('u')
                ->where('u.username = :username OR u.email = :email')
                ->setParameter('username', $username)
                ->setParameter('email', $username)
                ->getQuery()
                ->getOneOrNullResult();

            if (null === $user) {
                $message = sprintf(
                    'Unable to find an active admin AppBundle:User object identified by "%s".',
                    $username
                );
                throw new UsernameNotFoundException($message);
            }

            return $user;
        }
    }

.. versionadded:: 2.8
    W Symfony 2.8 został wprowadzony interfejs
    :class:`Symfony\\Bridge\\Doctrine\\Security\\User\\UserLoaderInterface`.
    Wcześniej trzeba było implementować ``Symfony\Component\Security\Core\User\UserProviderInterface``.

Więcej szczegółów dotyczących tych metod znajdziesz analizując interfejs
:class:`Symfony\\Component\\Security\\Core\\User\\UserProviderInterface`.

.. tip::

    Nie zpomnij dodać klasę repozytorium do
    :ref:`definicji mapowania swojej encji <book-doctrine-custom-repository-classes>`.

Na zakończenie po prostu usń klucz ``property`` z konfiguracji dostawcy uzytkowników
w ``security.yml``:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # bin/config/security.yml
        security:
            # ...

            providers:
                our_db_provider:
                    entity:
                        class: AppBundle:User

    .. code-block:: xml
       :linenos:

        <!-- bin/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd">

            <config>
                <!-- ... -->

                <provider name="our_db_provider">
                    <entity class="AppBundle:User" />
                </provider>
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // bin/config/security.php
        $container->loadFromExtension('security', array(
            // ...

            'providers' => array(
                'our_db_provider' => array(
                    'entity' => array(
                        'class' => 'AppBundle:User',
                    ),
                ),
            ),
        ));

Powiadamia to Symfony, aby nie wykonywał automatycznie zapytania dla obiektu User.
Zmiast tego ma być wywoływana  metoda ``loadUserByUsername()`` z ``UserRepository``
gdy ktoś się loguje.

.. _`cookbook-security-serialize-equatable`:

Wytłumaczenie serializacji i tego jak użytkownik jest zapisywany w sesji
------------------------------------------------------------------------

Jeśli interesuje Ciebie działanie metody ``serialize()`` wewnątrz klasy ``User``
lub to jak obiekt jest serializowany lub deserializowany, to ten rozdział jest
dla Ciebie. Jeśli nie, to go opuść.

Gdy użytkownik zostaje zalogowany, cały obiekt User zostaje serializowany do
sesji. Przy następnym żądaniu, obiekt User zostaje zdeserializowany.
Następnie, wartość właściwości ``id`` zostaje użyta do ponownego zapytania do
bazy danych dla świeżego obiektu. Na koniec, świeży obiekt User jest porównywany
z deserializowanym obiektem User, w celu upewnienia się, że reprezentuje tego
samego użytkownika. Na przyklad, jeśli wartości ``username`` w tych dwóch obiektach
nie są zgodne, to nastąpi wylogowanie uzytkownika, ze względów bezpieczeństwa.

Mimo że to wszystko odbywa się automatycznie, to istnieje kilka skutków ubocznych.

Po pierwsze, trzeba mieć dostęp do interfejsu PHP :phpclass:`Serializable` i jego
metod ``serialize`` i ``unserialize``, które umożliwiają serializowanie obiektów
klasy ``User`` do sesji. To może ale nie musi być potrzebne, w zależności od
konfiguracji, ale zainstalowanie tego inerfesu, to dobry pomysł. Teoretycznie,
serializować trzeba tylko ``id``, ponieważ metoda
:method:`Symfony\\Bridge\\Doctrine\\Security\\User\\EntityUserProvider::refreshUser`
odświeża obiekt użytkownika przy każdym żądaniu wykorzystując właśnie ``id``
(tak jak wyjaśniono to powyżej). To daje "świeży" obiekt User.

Lecz Symfony również uzywa włażciwości ``username``, ``salt`` i ``password`` do
zweryfikowania, czy uzytkownik nie zmienił się pomiędzy żądaniami (wywołuje to
również metody ``AdvancedUserInterface``, jeśli ten interfejs został zaimplementowany).
Błąd w serializacji spowoduje możliwość wylogowywania przy każdym żądaniu.
Jeśli klasa User implementuje
:class:`Symfony\\Component\\Security\\Core\\User\\EquatableInterface`,
wtedy zamiast sprawdzania tych właściwości, wywoływana jest po prostu metoda
``isEqualTo`` można sprawdzić takie właściwości, jakie się chcesz. Jeśli tego nie
rozumiesz, to najlepiej nie implementuj tego interfejsu.

.. _fikstur: https://symfony.com/doc/master/bundles/DoctrineFixturesBundle/index.html
.. _FOSUserBundle: https://github.com/FriendsOfSymfony/FOSUserBundle
