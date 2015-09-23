.. highlight:: php
   :linenothreshold: 2

.. index::
   single: walidacja

Walidacja
=========

Dane wprowadzone do formularzy wymagają weryfikacji. Dane wymagają również sprawdzenia
przed ich zapisem do bazy danych lub przekazaniem do serwisu internetowego.
Lecz nie chodzi tu tylko o prostą weryfikację danych. W tworzeniu aplikacji internetowych,
niezbędne jest wykazanie, że dane wprowadzane do aplikacji prowadzą do właściwych
wyników - to jest właśnie **walidacja**. `Walidacja`_ jest niezbędnym elementem
prawie wszystkich współczesnych procesów wytwórczych.  

Symfony dostarcza komponent `Validator`_, dzięki któremu to zadanie jest łatwe
 i zrozumiałe. Komponent ten oparty jest o `specyfikację JSR303 Bean Validation`_.
 

.. index:
   single: walidacja; podstawy

Podstawy walidacji
------------------

Najlepszą sposobem do zrozumienia walidacji jest zobaczenie jej w działaniu.
Aby zacząć, załóżmy, że utworzyliśmy zwykły obiekt PHP, który musi się użyć gdzieś
w swojej aplikacji:

.. code-block:: php
   :linenos:
   
    // src/AppBundle/Entity/Author.php
    namespace AppBundle\Entity;

    class Author
    {
        public $name;
    }

Jak dotąd, jest to tylko zwyczajna klasa, która robi coś w aplikacji.
Celem walidacji jest sprawdzenie, czy zawartość obiektu jest prawidłowa.
Aby to działało, trzeba skonfigurować listę reguł (zwanych
 :ref:`ograniczeniami (ang. constraints) <validation-constraints>`),
 które obiekt musi spełniać, aby przejść walidację. Reguły te mogą być określone
 w wielu różnych formatach (YAML, XML, adnotacje, czy PHP).

Na przykład, aby zagwarantować, że właściwość ``$name`` nie jest pusta,
trzeba dodać następujący kod:

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\NotBlank()
             */
            public $name;
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\Author:
            properties:
                name:
                    - NotBlank: ~

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\Author">
                <property name="name">
                    <constraint name="NotBlank" />
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;

        class Author
        {
            public $name;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new NotBlank());
            }
        }


.. tip::

    Właściwości chronione i prywatne mogą być również walidowane, podobnie jak
    metody akcesorów (getery) – zobacz :ref:`validator-constraint-targets`.

.. versionadded:: 2.7
    Począwszy od Symfony 2.7, ładowane są pliki ograniczeń w formacie XML i Yaml
    zlokalizowane w podkatalogu ``Resources/config/validation`` pakietu. Poprzednio
    ładowany był tylko plik ``Resources/config/validation.yml`` (lub ``.xml``).


.. index::
   single: walidacja; używanie walidatorów

Używanie usługi ``Validator``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Następnie, aby faktycznie walidować obiekt ``Author`` trzeba użyć metodę ``validate``
z usługi ``validator`` (klasy :class:`Symfony\\Component\\Validator\\Validator`).
Zadanie ``validator`` jest proste: odczytać ograniczenia i sprawdzić, czy dane
znajdujące się w obiekcie spełniają te ograniczenia. Jeśli walidacja się nie powiedzie,
zwracana jest nie pusta lista błedów
(klasa :class:`Symfony\\Component\\Validator\\ConstraintViolationList`).
Rozpatrzmy poniższy przykład z wnętrza kontrolera::
   
   // ...
    use Symfony\Component\HttpFoundation\Response;
    use AppBundle\Entity\Author;

    public function indexAction()
    {
        $author = new Author();
        // ... do something to the $author object

        $validator = $this->get('validator');
        $errors = $validator->validate($author);

        if (count($errors) > 0) {
            /*
             * Uses a __toString method on the $errors variable which is a
             * ConstraintViolationList object. This gives us a nice string
             * for debugging
             */
            $errorsString = (string) $errors;

            return new Response($errorsString);
        }

        return new Response('The author is valid! Yes!');
    }

Jeśli właściwość ``$name`` jest pusta, to można zobaczyć poniższy komunikat błędu:

.. code-block:: text

    AppBundle\Author.name:
        This value should not be blank

Jeśli wstawi się jakąś wartość do właściwości ``$name``, pojawi się komunikat o
sukcesie.

.. tip::

    Przeważnie nie trzeba korzystać bezpośrednio z usługi ``validator`` czy martwić
    się o wyświetlanie błędów. Zwykle  używa się walidację pośrednio podczas obsługiwania
    danych zgłaszanych w formularzu. Więcej informacji można znaleść w rozdziale
    :ref:`book-validation-forms`.

Możesz również przekazać kolekcję błędów do szablonu.

.. code-block:: php
   :linenos:

    if (count($errors) > 0) {
        return $this->render('author/validation.html.twig', array(
            'errors' => $errors,
        ));
    }
    
Jeśli zachodzi taka potrzeba, to można w szablonie wyprowadzić listę błędów:

.. configuration-block::

    .. code-block:: html+jinja

        {# app/Resources/views/author/validation.html.twig #}
        <h3>The author has the following errors</h3>
        <ul>
        {% for error in errors %}
            <li>{{ error.message }}</li>
        {% endfor %}
        </ul>

    .. code-block:: html+php

        <!-- app/Resources/views/author/validation.html.php -->
        <h3>The author has the following errors</h3>
        <ul>
        <?php foreach ($errors as $error): ?>
            <li><?php echo $error->getMessage() ?></li>
        <?php endforeach ?>
        </ul>


.. note::

    Każdy błąd walidacji (zwany "naruszeniem ograniczeń") reprezentowany jest przez
    obiekt :class:`Symfony\\Component\\Validator\\ConstraintViolation`.

.. index::
   single: walidacja; walidacja formularzy

.. _book-validation-forms:

Walidacja a formularze
~~~~~~~~~~~~~~~~~~~~~~

Usługa ``validator`` może być użyta do walidacji dowolnego obiektu w dowolnym momencie.
Jednakże w rzeczywistości najczęściej używa się walidatorów pośrednio,
podczas pracy z formularzami. Biblioteka formularzy Symfony używa usługi ``validator``
wewnętrznie do sprawdzenia odnośnego obiektu formularza po tym, jak wartości zostaną
zgłoszone i powiązane. Naruszenia ograniczeń dla obiektu są konwertowane do obiektu
``FormError``, który może być łatwo wyświetlany wraz z formularzem. Typowa procedura
wysłania formularza z poziomu kontrolera wygląda następująco::

    // ...
    use AppBundle\Entity\Author;
    use AppBundle\Form\AuthorType;
    use Symfony\Component\HttpFoundation\Request;

    // ...
    public function updateAction(Request $request)
    {
        $author = new Author();
        $form = $this->createForm(new AuthorType(), $author);

        $form->handleRequest($request);

        if ($form->isValid()) {
            // the validation passed, do something with the $author object

            return $this->redirectToRoute(...);
        }

        return $this->render('author/form.html.twig', array(
            'form' => $form->createView(),
        ));
    }

.. note::

    W tym przykładzie użyto klasę formularza ``AuthorType``, która nie jest
    tutaj pokazana.

W celu uzyskania więcej informacji zobacz do rozdziału :doc:`Formularze </book/forms>`.

.. index::
   pair: walidacja; konfiguracja

.. _book-validation-configuration:

Konfiguracja
------------

Walidator Symfony jest domyślnie włączony, jednak gdy chce się stosować metodę
adnotacji, to należy określić to w ograniczeniach:

.. configuration-block::

    .. code-block:: yaml

        # app/config/config.yml
        framework:
            validation: { enable_annotations: true }

    .. code-block:: xml

        <!-- app/config/config.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <container xmlns="http://symfony.com/schema/dic/services"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:framework="http://symfony.com/schema/dic/symfony"
            xsi:schemaLocation="http://symfony.com/schema/dic/services http://symfony.com/schema/dic/services/services-1.0.xsd
                http://symfony.com/schema/dic/symfony http://symfony.com/schema/dic/symfony/symfony-1.0.xsd">

            <framework:config>
                <framework:validation enable-annotations="true" />
            </framework:config>
        </container>

    .. code-block:: php

        // app/config/config.php
        $container->loadFromExtension('framework', array(
            'validation' => array(
                'enable_annotations' => true,
            ),
        ));

.. index::
   single: walidacja; ograniczenia
   pair: walidacja; asercje

.. _validation-constraints:

Ograniczenia
------------

Usługa ``validator`` jest zaprojektowana do weryfikacji obiektów w oparciu
o **ograniczenia** (*ang. ``constraints``*). W celu walidacji obiektu trzeba tylko
odwzorować jedno lub więcej ograniczeń na tą klasę  i następnie przekazać to do
usługi ``validator``.

Ograniczenie to prosty obiekt PHP, który wykonuje wyrażenie asercyjne (*ang.
assertive statement*). `Asercja`_ w programowaniu, to wyrażenie lub metoda pozwalająca
sprawdzić  prawdziwość twierdzeń dokonanych odnośnie jakichś aspektów systemu lub
jego elementów. W prawdziwym życiu, takim twierdzeniem może być zdanie: "Ciasto
nie może być spalone". W Symfony, ograniczenia są podobne: są to twierdzenia
(asercje), że warunek jest spełniony.

Obsługiwane ograniczenia
~~~~~~~~~~~~~~~~~~~~~~~~

Pakiety Symfony udostępniają większość z powszechnie używanych ograniczeń:

.. include:: /reference/constraints/map.rst.inc

Można również stworzyć własne reguły. Ten temat jest szerzej omówiony
w artykule ":doc:`/cookbook/validation/custom_constraint`".

.. index::
   single: walidacja; konfiguracja

.. _book-validation-constraint-configuration:

Konfiguracja ograniczeń
~~~~~~~~~~~~~~~~~~~~~~~

Niektóre ograniczenia, takie jak :doc:`NotBlank </reference/constraints/NotBlank>,
są proste, podczas gdy inne, jak np. :doc:`Choice </reference/constraints/Choice>
mają kilka dostępnych opcji konfiguracji. Załóżmy, że klasa ``Author`` ma właściwość
``gender``, które może przyjmować wartość "kobieta" albo "mężczyzna" albo "inny":

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\Choice(
             *     choices = { "male", "female", "other" },
             *     message = "Choose a valid gender."
             * )
             */
            public $gender;

            // ...
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\Author:
            properties:
                gender:
                    - Choice: { choices: [male, female, other], message: Choose a valid gender. }
                # ...

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\Author">
                <property name="gender">
                    <constraint name="Choice">
                        <option name="choices">
                            <value>male</value>
                            <value>female</value>
                            <value>other</value>
                        </option>
                        <option name="message">Choose a valid gender.</option>
                    </constraint>
                </property>

                <!-- ... -->
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            public $gender;

            // ...

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                // ...

                $metadata->addPropertyConstraint('gender', new Assert\Choice(array(
                    'choices' => array('male', 'female', 'other'),
                    'message' => 'Choose a valid gender.',
                )));
            }
        }


.. _validation-default-option:

Opcje ograniczeń mogą być również przekazywane w tablicy. Niektóre ograniczenia
pozwalają także przekazać wartość "domyślną" opcji zamiast tablicy.
W przypadku ograniczenia ``Choice`` opcje ``choices`` może zostać określona
w ten sposób.

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\Choice({"male", "female", "other"})
             */
            protected $gender;

            // ...
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\Author:
            properties:
                gender:
                    - Choice: [male, female, other]
                # ...

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\Author">
                <property name="gender">
                    <constraint name="Choice">
                        <value>male</value>
                        <value>female</value>
                        <value>other</value>
                    </constraint>
                </property>

                <!-- ... -->
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            protected $gender;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                // ...

                $metadata->addPropertyConstraint(
                    'gender',
                    new Assert\Choice(array('male', 'female', 'other'))
                );
            }
        }

Ma to jedynie na celu ułatwienie i przyspieszenie konfiguracji najczęściej
używanych opcji ograniczeń.

Jeśli czasem nie będziesz pewien jak określić opcję, sprawdź dokumentację API
dla ograniczeń lub postępuj bezpiecznie przekazując w tablicy opcji (pierwsza
powyżej pokazana metoda).


Tłumaczenie komunikatów ograniczeń
----------------------------------

Pełną informację o tłumaczeniu komunikatów ograniczeń znajdziesz w dokumencie
:ref:`book-translation-constraint-messages`.


.. index::
   single: walidacja; cele ograniczeń

.. _validator-constraint-targets:

Cele ograniczeń
---------------

Ograniczenia można stosować do właściwości klasy (np. ``name``) lub publicznych
metod aksesorów (np.``getFullName``). Pierwsze zastosowanie jest najbardziej
rozpowszechnione i łatwe w użyciu, lecz drugie pozwala określić bardziej złożone
reguły walidacji.

.. index::
   single: walidacja; właściwości ograniczeń

.. _validation-property-target:

Właściwości
~~~~~~~~~~~

Walidacja właściwości klas jest dość rozpowszechnioną a techniką walidacji.
Symfony pozwala sprawdzać prywatne, chronione i publiczne właściwości.
Następujący listing pokazuje jak skonfigurować właściwość ``$firstName``
klasy ``Author``, której wartość powinna mieć co najmniej 3 znaki.

.. configuration-block::

    .. code-block:: php-annotations

        // AppBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\NotBlank()
             * @Assert\Length(min=3)
             */
            private $firstName;
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\Author:
            properties:
                firstName:
                    - NotBlank: ~
                    - Length:
                        min: 3

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\Author">
                <property name="firstName">
                    <constraint name="NotBlank" />
                    <constraint name="Length">
                        <option name="min">3</option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            private $firstName;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('firstName', new Assert\NotBlank());
                $metadata->addPropertyConstraint(
                    'firstName',
                    new Assert\Length(array("min" => 3))
                );
            }
        }


.. index::
   single: walidacja; ograniczenia dla akcesorów
   pair: walidacja; akcesory

Metody akcesory
~~~~~~~~~~~~~~~

Ograniczenia mogą również być stosowane do zwrócenia wartości metody.
Symfony umożliwia dodanie ograniczenia do jakiejkolwiek publicznej metody,
której nazwa zaczyna się od "get" lub "is" lub "has". W tym podręczniku metody te
są kreślane jako (getter, isser i haser).

Zaletą tej techniki jest to, że pozwala dynamicznie walidować obiekt. Przykładowo
załóżmy, że chcemy się upewnić, że pole hasła nie zgadza się z imieniem użytkownika
(w celach bezpieczeństwa). Można to uczynić przez stworzenie metody ``isPasswordLegal``
i następnie zrobić załóżenie, że metoda ta musi zwrócić ``true``:

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            /**
             * @Assert\True(message = "The password cannot match your first name")
             */
            public function isPasswordLegal()
            {
                // ... return true or false
            }
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\Author:
            getters:
                passwordLegal:
                    - "True": { message: "The password cannot match your first name" }

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\Author">
                <getter property="passwordLegal">
                    <constraint name="True">
                        <option name="message">The password cannot match your first name</option>
                    </constraint>
                </getter>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/Author.php

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class Author
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addGetterConstraint('passwordLegal', new Assert\True(array(
                    'message' => 'The password cannot match your first name',
                )));
            }
        }

Teraz utwórzmy metodę ``isPasswordLegal()`` i dołączmy potrzebną logikę::

    public function isPasswordLegal()
    {
        return ($this->firstName != $this->password);
    }

.. note::

    Niektórzy z czytelników mogli zauważyć, że przedrostek akcesora ("get", "is" lub "has")
    jest pomijany w odwzorowaniu. Pozwala to później przenieść ograniczenie do
    właściwości o tej samej nazwie (lub odwrotnie) bez zmiany logiki walidacji.

.. _validation-class-target:

Klasy
~~~~~

Niektóre ograniczenia mogą być zastosowanie do walidacji całej klasy. Na przykład,
ograniczenie :doc:`Callback </reference/constraints/Callback>` jest ogólnym
ograniczeniem stosowanym do całej klasy. Podczas walidacji klasy, wykonywane
są metody określone przez ograniczenie, więc każda może dostarczyć niestandardowej
walidacji.

.. _book-validation-validation-groups:

Grupy walidacji
---------------

Dotychczas byliśmy w stanie dodać ograniczenia do klasy i zapytać, czy klasa spełnia
wszystkie określone ograniczenia. Jednak w niektórych przypadkach zachodzi potrzeba
potwierdzenia tylko niektórych z tych ograniczeń w klasie. Aby to wykonać, trzeba
zorganizować każde z ograniczeń w jednej lub więcej "grup walidacyjnych" i następnie
zastosować walidację tylko na jednej z takich grup.

Przykładowo załóżmy, że mamy klasę User, która jest stosowana zarówno podczas
rejestracji użytkownika jak i podczas aktualizowania jego informacji kontaktowych:

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        use Symfony\Component\Security\Core\User\UserInterface;
        use Symfony\Component\Validator\Constraints as Assert;

        class User implements UserInterface
        {
            /**
            * @Assert\Email(groups={"registration"})
            */
            private $email;

            /**
            * @Assert\NotBlank(groups={"registration"})
            * @Assert\Length(min=7, groups={"registration"})
            */
            private $password;

            /**
            * @Assert\Length(min=2)
            */
            private $city;
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\User:
            properties:
                email:
                    - Email: { groups: [registration] }
                password:
                    - NotBlank: { groups: [registration] }
                    - Length: { min: 7, groups: [registration] }
                city:
                    - Length:
                        min: 2

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="
                http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd
            ">

            <class name="AppBundle\Entity\User">
                <property name="email">
                    <constraint name="Email">
                        <option name="groups">
                            <value>registration</value>
                        </option>
                    </constraint>
                </property>

                <property name="password">
                    <constraint name="NotBlank">
                        <option name="groups">
                            <value>registration</value>
                        </option>
                    </constraint>
                    <constraint name="Length">
                        <option name="min">7</option>
                        <option name="groups">
                            <value>registration</value>
                        </option>
                    </constraint>
                </property>

                <property name="city">
                    <constraint name="Length">
                        <option name="min">7</option>
                    </constraint>
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class User
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('email', new Assert\Email(array(
                    'groups' => array('registration'),
                )));

                $metadata->addPropertyConstraint('password', new Assert\NotBlank(array(
                    'groups' => array('registration'),
                )));
                $metadata->addPropertyConstraint('password', new Assert\Length(array(
                    'min'    => 7,
                    'groups' => array('registration'),
                )));

                $metadata->addPropertyConstraint('city', new Assert\Length(array(
                    "min" => 3,
                )));
            }
        }

W tej konfiguracji są trzy grupy walidacyjne:

* ``Default`` - zawiera ograniczenia nieprzypisane do żadnej innej grupy;

* ``User`` - zawiera ograniczenia, które należą do grupy ``Default`` (grupa ta
   jest przydatna dla :ref:`sekwencji grup <book-validation-group-sequence>`);

* ``registration`` - zawiera ograniczenia wyłącznie dla pól ``email`` i ``password``.

Ograniczenia w grupie ``Default`` jakiejśc klasy są to ograniczenia, które nie
mają jawnie skonfigurowanej grupy albo takie, które są skonfigurowane do grupy,
która odpowiada klasie ``Default`` lub nazywa się ``Default``.

.. caution::

    Podczas walidacji *tylko* obiektu User, nie ma różnicy pomiędzy grupą``Default``
    a grupą ``User``. Jednak taka różnica będzie, jeśli ``User`` ma osadzone obiekty.
    Na przykład, wyobraźmy sobie, że ``User`` ma właściwość ``address``, która
    zawiera jakiś obiekt ``Address`` i że dodaliśmy do tej właściwości ograniczenie
    :doc:`Valid </reference/constraints/Valid>`, tak więc obiekt ten będzie też 
    sprawdzany podczas walidacji obiektu ``User``.

    Jeśli sprawdza się ``User`` używając grupy ``Default`, to będą użyte wszystkie
    ograniczenia ustawione na klasie ``Address``, która znajduje się w grupie
    ``Default``. Lecz jeśli sprwdza się ``User`` używając grupy walidacyjnej
    ``User``, to walidowane będą tylko ograniczenia ustawione na klasie ``Address``
    w grupie ``User`.

    Innymi słowami, grupa ``Default`` i grupa o nazwie klasy (np. ``User``) są
    identyczne, z wyjątkiem przypadku, gdy klasa jest osadzona w innym obiekcie,
    który w rzeczywistości jest tym, co podlega walidacji.

    Jeśli użyło sie dziedziczenia (np. ``User extends BaseUser``) i sprawdza się
    grupę z nazwą podklasy (np. ``User``), to sprawdzane będą wszystkoe ograniczenia
    w ``User`` i ``BaseUser`. Jednakże, jeśli sprawdzanie realizuje się z wykorzystaniem
    klasy bazowej (tj. ``BaseUser``), to sprawdzane będą tylko ograniczenia w klasie
    ``BaseUser``.

Aby poinstruować walidator o stosowaniu określonej grupy, trzeba przekazać jedną
lub więcej nazw grup jako trzeci argument metody ``validate()``::

    // If you're using the new 2.5 validation API (you probably are!)
    $errors = $validator->validate($author, null, array('registration'));

    // If you're using the old 2.4 validation API, pass the group names as the second argument
    // $errors = $validator->validate($author, array('registration'));
    
Jeśli nie zostanie określona żadna grupa, to będą używane ograniczenia należące
do grupy ``Default``.

Oczywiście będziesz zazwyczaj używał walidacji pośrednio, poprzez bibliotekę formularzy.
Więcej informacji o tym jak używać grup walidacyjnych w formularzach znajdziesz w rozdziale
:ref:`book-forms-validation-groups`.

.. _book-validation-group-sequence:

Sekwencja grup
--------------

W niektórych przypadkach zachodzi potrzeba etapowego sprawdzenia grup. Do wykonania
tego należy użyć funkcjonalności ``GroupSequence``. W takim przypadku obiekt określa
sekwencję grup a następnie grupy w takiej sekwencji są sprawdzane po koleji.

Dla przykładu załóżmy, że mamy klasę ``User`` i chcemy sprawdzić, że *username*
i *password* są różne tylko, gdy wszystkie inne dane przechodzą walidację (w celu
uniknięcia wielu komunikatów o błędach).

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        use Symfony\Component\Security\Core\User\UserInterface;
        use Symfony\Component\Validator\Constraints as Assert;

        /**
         * @Assert\GroupSequence({"User", "Strict"})
         */
        class User implements UserInterface
        {
            /**
            * @Assert\NotBlank
            */
            private $username;

            /**
            * @Assert\NotBlank
            */
            private $password;

            /**
             * @Assert\True(message="The password cannot match your username", groups={"Strict"})
             */
            public function isPasswordLegal()
            {
                return ($this->username !== $this->password);
            }
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\User:
            group_sequence:
                - User
                - Strict
            getters:
                passwordLegal:
                    - "True":
                        message: "The password cannot match your username"
                        groups: [Strict]
            properties:
                username:
                    - NotBlank: ~
                password:
                    - NotBlank: ~

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\User">
                <property name="username">
                    <constraint name="NotBlank" />
                </property>

                <property name="password">
                    <constraint name="NotBlank" />
                </property>

                <getter property="passwordLegal">
                    <constraint name="True">
                        <option name="message">The password cannot match your username</option>
                        <option name="groups">
                            <value>Strict</value>
                        </option>
                    </constraint>
                </getter>

                <group-sequence>
                    <value>User</value>
                    <value>Strict</value>
                </group-sequence>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints as Assert;

        class User
        {
            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('username', new Assert\NotBlank());
                $metadata->addPropertyConstraint('password', new Assert\NotBlank());

                $metadata->addGetterConstraint('passwordLegal', new Assert\True(array(
                    'message' => 'The password cannot match your first name',
                    'groups'  => array('Strict'),
                )));

                $metadata->setGroupSequence(array('User', 'Strict'));
            }
        }

W tym przykładzie najpierw sprawdzamy wszystkie ograniczenia w grupie ``User``
(która jest taka sama jak grupa ``Default``). Gdy wszystkie ograniczenia w tej
grupie są prawidłowe i tylko wtedy, sprawdzana jest druga grupa ``Strict``.

.. caution::

    Jak już to widzieliśmy w poprzednim rozdziale, grupa ``Default`` i grupa
    z nazwą klasy (np. ``User``) były identyczne.
    Jednak, gdy używa się sekwencji grup, nie są one już identyczne. Grupa ``Default``
    będzie teraz odnościć sie do sekwencji grup a nie do tych wszystkich ograniczeń,
    które  nie należą do żadnej grupy.

    Oznacza to, że trzeba użyć grupe ``{ClassName}`` (np. ``User``), gdy określa
    się sekwencję grup. Po zastosowaniu ``Default``, otrzyma się nieskończoną
    rekurencję (jako ze grupa ``Default`` odnosi się do sekwencji grup, która
    bedzie zawierać grupę ``Default`` z odwołaniami do tej samej sekwencji grup, ...).

Dostawcy sekwencji grup
~~~~~~~~~~~~~~~~~~~~~~~

Przyjmijmy, że mamy encję ``User``, która może być zwykłym użytkownikiem lub
użytkownikiem premium. Gdy encja jest użytkownikiem premium, to powinno się do
niej dodać kilka specyficznych ograniczeń (np. szczegóły karty kredytowej).
Dla dynamicznego określania, które grupy powinny zostać aktywowane, można utworzyć
dostawcę sekwencji grup (*ang. Group Sequence Provider*). Najpierw utwórzmy encję
i nowe ograniczenie o nazwie``Premium``:

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        use Symfony\Component\Validator\Constraints as Assert;

        class User
        {
            /**
             * @Assert\NotBlank()
             */
            private $name;

            /**
             * @Assert\CardScheme(
             *     schemes={"VISA"},
             *     groups={"Premium"},
             * )
             */
            private $creditCard;

            // ...
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\User:
            properties:
                name:
                    - NotBlank: ~
                creditCard:
                    - CardScheme:
                        schemes: [VISA]
                        groups: [Premium]

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\User">
                <property name="name">
                    <constraint name="NotBlank" />
                </property>

                <property name="creditCard">
                    <constraint name="CardScheme">
                        <option name="schemes">
                            <value>VISA</value>
                        </option>
                        <option name="groups">
                            <value>Premium</value>
                        </option>
                    </constraint>
                </property>

                <!-- ... -->
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        use Symfony\Component\Validator\Constraints as Assert;
        use Symfony\Component\Validator\Mapping\ClassMetadata;

        class User
        {
            private $name;
            private $creditCard;

            // ...

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new Assert\NotBlank());
                $metadata->addPropertyConstraint('creditCard', new Assert\CardScheme(
                    'schemes' => array('VISA'),
                    'groups'  => array('Premium'),
                ));
            }
        }


Teraz zmieńmy klasę ``User``, aby zaimplementować interfejs
:class:`Symfony\\Component\\Validator\\GroupSequenceProviderInterface` i dodajmy 
metodę
:method:`Symfony\\Component\\Validator\\GroupSequenceProviderInterface::getGroupSequence`,
która powinna zwracać tablicę group do zastosowania::

    // src/AppBundle/Entity/User.php
    namespace AppBundle\Entity;

    // ...
    use Symfony\Component\Validator\GroupSequenceProviderInterface;

    class User implements GroupSequenceProviderInterface
    {
        // ...

        public function getGroupSequence()
        {
            $groups = array('User');

            if ($this->isPremium()) {
                $groups[] = 'Premium';
            }

            return $groups;
        }
    }

W końcu, trzeba powiadomić komponent Validator , że klasa ``User`` dostarcza
sekwencję grup do zastosowania w walidacji:

.. configuration-block::

    .. code-block:: php-annotations

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        // ...

        /**
         * @Assert\GroupSequenceProvider
         */
        class User implements GroupSequenceProviderInterface
        {
            // ...
        }

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\User:
            group_sequence_provider: true

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping
                http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\User">
                <group-sequence-provider />
                <!-- ... -->
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/User.php
        namespace AppBundle\Entity;

        // ...
        use Symfony\Component\Validator\Mapping\ClassMetadata;

        class User implements GroupSequenceProviderInterface
        {
            // ...

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->setGroupSequenceProvider(true);
                // ...
            }
        }


.. index::
   single: walidacja; walidacja wartości
   single: walidacja; walidacja tablic

.. _book-validation-raw-values:

Walidacja wartości i tablic
---------------------------

Dotąd widzieliśmy, jak można sprawdzać całe obiekty. Lecz czasami zachodzi potrzeba
sprawdzenia tylko pojedynczej wartości, jak na przykład łańcucha tekstowego, który
powinien być prawidłowym adresem e-mail. W rzeczywistości jest to bardzo łatwe do
zrobienia. Wewnątrz kontrolera wygląda to podobnie do tego::

    // ...
    use Symfony\Component\Validator\Constraints as Assert;

    // ...
    public function addEmailAction($email)
    {
        $emailConstraint = new Assert\Email();
        // all constraint "options" can be set this way
        $emailConstraint->message = 'Invalid email address';

        // use the validator to validate the value
        // If you're using the new 2.5 validation API (you probably are!)
        $errorList = $this->get('validator')->validate(
            $email,
            $emailConstraint
        );

        // If you're using the old 2.4 validation API
        /*
        $errorList = $this->get('validator')->validateValue(
            $email,
            $emailConstraint
        );
        */

        if (0 === count($errorList)) {
            // ... this IS a valid email address, do something
        } else {
            // this is *not* a valid email address
            $errorMessage = $errorList[0]->getMessage();

            // ... do something with the error
        }

        // ...
    }

Wywołując w walidatorze metodę ``validate`` można przekazać tam surową wartość
i obiekt ograniczenia, jakie chce się walidować. Pełną listę dostępnych ograniczeń,
z pełną nazwą klasy dla każdego ograniczenia, znajdziesz w rozdziale
:doc:`Informacje o ograniczeniach </reference/constraints>`.

Metoda ``validate`` zwraca obiekt
:class:`Symfony\\Component\\Validator\\ConstraintViolationList`,
który zachowuje się zupełnie jak tablica błędów. Każdy błąd w kolekcji jest obiektem
:class:`Symfony\\Component\\Validator\\ConstraintViolation` który przechowuje
komunikat błędu w swojej metodzie ``getMessage``.

Wnioski końcowe
---------------

Usługa ``validator`` Symfony jest zaawansowanym narzędziem, które można wykorzystać
do zagwarantowania poprawności danych każdego obiektu. Siła walidacji bierze się
z "ograniczeń", które są regułami jakie muszą spełniać właściwości lub metody
akcesory w obiekcie. Najczęściej będziesz stosował framework walidacyjny pośrednio,
podczas używania formularzy, pamiętając, że walidacja może być stosowana gdziekolwiek,
w celu sprawdzenia poprawności danych dowolnego obiektu.

Dalsza lektura
--------------

* :doc:`Jak utworzyć własne ograniczenia walidacyjne</cookbook/validation/custom_constraint>`

.. _`Validator`: https://github.com/symfony/Validator
.. _`JSR303 Bean Validation specification`: http://jcp.org/en/jsr/detail?id=303
.. _`Walidacja`: http://pl.wikipedia.org/wiki/Walidacja_(technika)
.. _`Asercja`: http://pl.wikipedia.org/wiki/Asercja_(informatyka)