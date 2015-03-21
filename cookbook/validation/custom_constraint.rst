.. index::
   single: Validation; Custom constraints

Jak stworzyć własny walidator ograniczenia
============================================

Możesz stworzyć własne ograniczenie rozszerzając klasę bazową ograniczeń
:class:`Symfony\\Component\\Validator\\Constraint`. Dla przykładu, stworzymy 
prostą walidację, która będzie sprawdzać czy dany ciąg znaków zawiera wyłącznie 
alfanumeryczne znaki.

Tworzenie klasy ograniczenia
-----------------------------
W pierwszym kroku stworzymy klasę ograniczenia rozszerzając klasę abstrakcyjną
:class:`Symfony\\Component\\Validator\\Constraint`:: 

    // src/AppBundle/Validator/Constraints/ContainsAlphanumeric.php
    namespace AppBundle\Validator\Constraints;

    use Symfony\Component\Validator\Constraint;

    /**
     * @Annotation
     */
    class ContainsAlphanumeric extends Constraint
    {
        public $message = 'Ciąg znaków "%string%" zawiera niedozwolone znaki: może zawierać wyłącznie litery bądź cyfry.';
    }

.. note::

    Adnotacja ``@Annotation`` jest potrzeba, jeżeli chcemy aby nasze ograniczenie,
    można było używać w klasie poprzez adnotację. Opcje dla Twojego ograniczenia reprezentowane
    są jako publiczne właściwości klasy.

Tworzenie Walidatora ograniczenia
-----------------------------

Jak widzisz, klasa ograniczenia jest dość minimalna. W rzeczywistości walidacja wykonywana jest
przez klasę walidatora ograniczenia. Klasa walidatora ograniczenia określana jest przez metodę ``validatedBy()``, 
która zawiera prostą logikę::

    // in the base Symfony\Component\Validator\Constraint class
    public function validatedBy()
    {
        return get_class($this).'Validator';
    }

Inaczej mówiąc, jeżeli stowrzysz właną klasę ``Ograniczenia`` (np. ``MojeOgraniczenie``),
Symfony automatycznie będzię szukał klasy ``MojeOgraniczenieValidator``, która
wykona walidację.

Klasa walidatora jest również prosta i wymaga jednej metody ``validate()``::

    // src/AppBundle/Validator/Constraints/ContainsAlphanumericValidator.php
    namespace AppBundle\Validator\Constraints;

    use Symfony\Component\Validator\Constraint;
    use Symfony\Component\Validator\ConstraintValidator;

    class ContainsAlphanumericValidator extends ConstraintValidator
    {
        public function validate($value, Constraint $constraint)
        {
            if (!preg_match('/^[a-zA-Za0-9]+$/', $value, $matches)) {
                //  Jeżeli używasz nowej wersji API walidacji 2.5
                $this->context->buildViolation($constraint->message)
                    ->setParameter('%string%', $value)
                    ->addViolation();

                // Jeżel używasz starszej wersji API walicaji 2.4
                /*
                $this->context->addViolation(
                    $constraint->message,
                    array('%string%' => $value)
                );
                */
            }
        }
    }

Metoda ``validate`` nie musi zwracać jakiejkolwiek wartości. Zamiast tego,
należy dodać ``violation`` (naruszenie) do właściwości context walidatora, walidacja będzie poprawna
jeżeli nie wystąpią żadne naruszenia. Metoda ``buildViolation`` jako parametr przyjmuję
wiadomość z błędem i zwraca instancję :class:`Symfony\\Component\\Validator\\Violation\\ConstraintViolationBuilderInterface`,
Wywołanie metody ``addViolation`` dodaje naruszenie do context.

Użycie nowego Walidatora
-----------------------

Korzystanie z niestandardowych walidatorów jest tak proste jak z tych dostarczanych przez Symfony:  

.. configuration-block::

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\AcmeEntity:
            properties:
                name:
                    - NotBlank: ~
                    - AppBundle\Validator\Constraints\ContainsAlphanumeric: ~

    .. code-block:: php-annotations

        // src/AppBundle/Entity/AcmeEntity.php
        use Symfony\Component\Validator\Constraints as Assert;
        use AppBundle\Validator\Constraints as AcmeAssert;

        class AcmeEntity
        {
            // ...

            /**
             * @Assert\NotBlank
             * @AcmeAssert\ContainsAlphanumeric
             */
            protected $name;

            // ...
        }

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <?xml version="1.0" encoding="UTF-8" ?>
        <constraint-mapping xmlns="http://symfony.com/schema/dic/constraint-mapping"
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:schemaLocation="http://symfony.com/schema/dic/constraint-mapping http://symfony.com/schema/dic/constraint-mapping/constraint-mapping-1.0.xsd">

            <class name="AppBundle\Entity\AcmeEntity">
                <property name="name">
                    <constraint name="NotBlank" />
                    <constraint name="AppBundle\Validator\Constraints\ContainsAlphanumeric" />
                </property>
            </class>
        </constraint-mapping>

    .. code-block:: php

        // src/AppBundle/Entity/AcmeEntity.php
        use Symfony\Component\Validator\Mapping\ClassMetadata;
        use Symfony\Component\Validator\Constraints\NotBlank;
        use AppBundle\Validator\Constraints\ContainsAlphanumeric;

        class AcmeEntity
        {
            public $name;

            public static function loadValidatorMetadata(ClassMetadata $metadata)
            {
                $metadata->addPropertyConstraint('name', new NotBlank());
                $metadata->addPropertyConstraint('name', new ContainsAlphanumeric());
            }
        }

Jeżeli Twoje ograniczenie zawiera opcje, wtedy potrzeba ustawić właściwości klasy ograniczenia jako publiczne.
Dzięki temu opcję te będą mogły być konfigurowane tak samo jak opcje ograniczeń dostarczanych przez Symfony.

Ograniczenie walidatora z zależnościami
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Jeżeli ograniczenie walidatora posiada zależności, takie jak np. połączenie z bazą,
potrzeba będzie skonfigurować je jako usługa w kontenerze zależności. Usługa musi
zawierać tag ``validator.constraint_validator`` oraz atrybut ``alias``.

.. configuration-block::

    .. code-block:: yaml

        # app/config/services.yml
        services:
            validator.unique.your_validator_name:
                class: Fully\Qualified\Validator\Class\Name
                tags:
                    - { name: validator.constraint_validator, alias: alias_name }

    .. code-block:: xml

        <!-- app/config/services.xml -->
        <service id="validator.unique.your_validator_name" class="Fully\Qualified\Validator\Class\Name">
            <argument type="service" id="doctrine.orm.default_entity_manager" />
            <tag name="validator.constraint_validator" alias="alias_name" />
        </service>

    .. code-block:: php

        // app/config/services.php
        $container
            ->register('validator.unique.your_validator_name', 'Fully\Qualified\Validator\Class\Name')
            ->addTag('validator.constraint_validator', array('alias' => 'alias_name'));

Twoja klasa ograniczenia powinna teraz stosować alias jako referencja do walidatora ograniczenia::

    public function validatedBy()
    {
        return 'alias_name';
    }

Jak wyżej wspomniano, Symfony wyszuka klasę po nazwie ograniczenia, wraz z przyrostkiem ``Validator``.
Jeżeli Twoje ograniczenie walidatora zdefiniowane jest jako usługa, ważne jest aby napisać metodę
``validatedBy()``, aby zwrócić alias określony w usłudzę, inaczej Symfony nie będzie
korzystać z usługi walidatora ograniczenia i zainicjuje klasę bez żadnych zależności

Klasa walidatora ograniczenia
~~~~~~~~~~~~~~~~~~~~~~~~~~

Oprócz właściwości klasy, ograniczenie może posiadać zakres w którym dostarczany jest cel walidacji::

    public function getTargets()
    {
        return self::CLASS_CONSTRAINT;
    }

W tym przypadku, metoda walidatora ograniczenia ``validate()`` dostaje obiekt jako pierwszy argument::

    class ProtocolClassValidator extends ConstraintValidator
    {
        public function validate($protocol, Constraint $constraint)
        {
            if ($protocol->getFoo() != $protocol->getBar()) {
                //Jeżeli używasz nowe API walidacji 2.5
                $this->context->buildViolation($constraint->message)
                    ->atPath('foo')
                    ->addViolation();

                // Jeżeli używasz starą wersję API walidacji 2.4
                /*
                $this->context->addViolationAt(
                    'foo',
                    $constraint->message,
                    array(),
                    null
                );
                */
            }
        }
    }

Zauważ że klasa ograniczenia jest stosowana do całej klasy danej encji,
a nie do pojedynczej właściwości klasy,

.. configuration-block::

    .. code-block:: yaml

        # src/AppBundle/Resources/config/validation.yml
        AppBundle\Entity\AcmeEntity:
            constraints:
                - AppBundle\Validator\Constraints\ContainsAlphanumeric: ~

    .. code-block:: php-annotations

        /**
         * @AcmeAssert\ContainsAlphanumeric
         */
        class AcmeEntity
        {
            // ...
        }

    .. code-block:: xml

        <!-- src/AppBundle/Resources/config/validation.xml -->
        <class name="AppBundle\Entity\AcmeEntity">
            <constraint name="AppBundle\Validator\Constraints\ContainsAlphanumeric" />
        </class>
