SI.1-1. The software
avoids hard-coded
passwords.

SI.1-2. Software source
code does not contain
secrets.

SI.1-3. Authentication
mechanisms used by
the software employ
industry standard
security techniques and
avoid common security
weaknesses.

SI.1-4. The software
does not store
sensitive authentication
information, which may
include passwords or
keys, in source code
or publicly accessible
infrastructure.

SI.1-5. Any passwords or
sensitive authentication
information stored by
the software is stored in
accordance with current
best practices.

SI.2-1. The software
implements features,
configurations, and
protocols that establish
or support standard,
tested authentication
services.

SI.2-2. The software
is interoperable with
applicable common
industry standards for
identity management
and authentication.

SI.2-3. Authentication
controls fail securely.

PA.1-1. Software is
capable of validating the
integrity of a transmitted
patch or update.

PA.1-2. Software
includes a mechanism to
notify end users of patch
or update installation.

PA.1-3. Software reverts
to a known-good state
upon failed installation
of updates or security
patches

CS.1-1. Software
enables the use of
encryption to protect
sensitive data from
unauthorized disclosure
or modification.

CS.1-2. Software enables
the use of encryption
to protect the software
itself from tampering.

CS.1-3. Software does
not expose sensitive
data upon failure of
encryption mechanisms.

CS.2-1. Software avoids
custom encryption
algorithms and
implementations.

CS.2-2. Software enables
the use of authenticated
encryption.

CS.2-3. Cryptography
employed by the
software enables strong
algorithms.

CS.2-4. Cryptography
employed by the
software enables strong
key lengths.

CS.2-5. Encryption
capabilities employed
by the software are
configured to select
strong cipher modes and
exclude weak ciphers by
default.

CS.2-6. Software is
configured to disable or
prevent the use of weak
encryption algorithms
and key lengths.

CS.3-1. Software ensures
that cryptographic keys
can be securely stored
and managed, separate
from encrypted data.

CS.3-2. Software
includes a mechanism
to manage key and
certificate lifecycles.

CS.3-3. Software
includes a mechanism to
validate certificates.

LO.1-1. Software
differentiates between
monitoring logs and
auditing logs.

LO.1-2. Software is
capable of logging all
security-relevant failures,
errors, and exceptions.

LO.1-3. Software is
capable of logging
timestamp and
identifying information
associated with security
incidents and events.

LO.2-2. Logging
mechanisms include anti-
tamper protections.

LO.2-3. Logs do
not store sensitive
information, such
as unnecessary user
information, system
details, session
identifiers, or passwords.

LO.2-4. Software
logging mechanisms
employ input validation
and output encoding.

EE.1-1. Software
identifies predictable
exceptions and errors
that could occur during
software execution
and defines how the
software will handle each
instance.

EE.1-2. Software
defines how it will
handle unpredicted
exceptions and errors
and safeguards against
continued execution in
an insecure state.

EE.1-3. Notifications of
errors and exceptions
do not disclose sensitive
technical or human
information.

EE.2-1. Software is
designed to continue
operating in a degraded
manner until a threshold
is reached that
triggers orderly, secure
termination.

EE.2-2. In the case
of failure, software
reverts to secure default
states that preserve
confidentiality and
integrity.