Analysis of Alternatives.

The objective is to evaluate different technologies for the server and client.
Each code will have its own directory with a quick analysis here.

# Protobuf

Can be used for message packets.

Pros
+ language neutral code for defining packets
+ generate a lot of boilerplate code
+ Can be used for multiple languages
+ fairly size efficient

Cons:
- (small) learning curve
- Can squeeze (slightly) more size out of custom application
- Another dependency

# Server language

Unfortuantely, developing PoCs would take a time to evaluate.
Instead the following is from experience and internet research.

## C Server

Pros:
+ lot os libraries.
+ venerable
Cons:
- have to manually multithread
- notorious for memory leaks

## Rust Server
+ lots of libraries
* Far fewer memory issues.

Cons:
- Still have to manually multithread, but not as bad as C.
- fanboy problem

## Go Server


# Client language
Unfortuantely, developing PoCs would take a time to evaluate.
Instead the following is from experience and internet research.

TBW
