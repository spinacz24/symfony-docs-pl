.. highlight:: php
   :linenothreshold: 2

.. index::
    single: bezpieczeństwo; nazwany koder
    single: szyfrowanie hasła; dynamiczne ustawianie algorytmu kodowania
    single: szyfrowanie hasła; nazwany koder  

Jak dynamicznie wybierać algorytm kodowania hasła
=================================================

Zazwyczaj, dla wszystkich instancji określonej klasy stosuje się ten sam koder
hasła:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...
            encoders:
                Symfony\Component\Security\Core\User\User: sha512

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8"?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd"
        >
            <config>
                <!-- ... -->
                <encoder class="Symfony\Component\Security\Core\User\User"
                    algorithm="sha512"
                />
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...
            'encoders' => array(
                'Symfony\Component\Security\Core\User\User' => array(
                    'algorithm' => 'sha512',
                ),
            ),
        ));

Innym rozwiązaniem jest użycie "nazwanego" kodera i dynamiczne określanie, który
koder chce się użyć w danej sytuacji.

W poprzednim przykładzie, ustawiliśmy algorytm ``sha512`` dla encji ``Acme\UserBundle\Entity\User``.
Może to być bezpieczne dla zwykłych użytkowników, ale co, gdy będziemy potrzebować
silniejszego algorytmu dla administratorów, na przykład ``bcrypt``. Można to zrobić
w następujący sposób:

.. configuration-block::

    .. code-block:: yaml
       :linenos:

        # app/config/security.yml
        security:
            # ...
            encoders:
                harsh:
                    algorithm: bcrypt
                    cost: 15

    .. code-block:: xml
       :linenos:

        <!-- app/config/security.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <srv:container xmlns="http://symfony.com/schema/dic/security"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:srv="http://symfony.com/schema/dic/services"
            xsi:schemaLocation="http://symfony.com/schema/dic/services
                http://symfony.com/schema/dic/services/services-1.0.xsd"
        >

            <config>
                <!-- ... -->
                <encoder class="harsh"
                    algorithm="bcrypt"
                    cost="15" />
            </config>
        </srv:container>

    .. code-block:: php
       :linenos:

        // app/config/security.php
        $container->loadFromExtension('security', array(
            // ...
            'encoders' => array(
                'harsh' => array(
                    'algorithm' => 'bcrypt',
                    'cost'      => '15'
                ),
            ),
        ));

Stworzy to koder o nazwie ``harsh``. Jeśli instacja ``User`` ma używać ten koder,
to klasa musi implementować interfejs
:class:`Symfony\\Component\\Security\\Core\\Encoder\\EncoderAwareInterface`.
Interfejs ten wymaga jednej metody, ``getEncoderName``, która powinna zwracać
nazwę kodera, jaki ma być zastosowany::

    // src/Acme/UserBundle/Entity/User.php
    namespace Acme\UserBundle\Entity;

    use Symfony\Component\Security\Core\User\UserInterface;
    use Symfony\Component\Security\Core\Encoder\EncoderAwareInterface;

    class User implements UserInterface, EncoderAwareInterface
    {
        public function getEncoderName()
        {
            if ($this->isAdmin()) {
                return 'harsh';
            }

            return null; // use the default encoder
        }
    }
