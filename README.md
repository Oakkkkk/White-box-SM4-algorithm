# README

This repository provides implementations of the Xiao-Lai White-box SM4 encryption programs and a novel Dynamic White-box SM4 encryption scheme.

## Xiao-Lai White-box SM4

The original white-box SM4 encryption program is available in the `Xiao-Lai-White-box-SM4-master` folder. This implementation serves as a baseline for traditional static white-box encryption approaches.

## Dynamic White-box SM4

In addition to the Xiao-Lai implementation, we provide a new Dynamic White-box SM4 encryption program in this repository. The dynamic white-box scheme demonstrates significant advantages in dynamic encryption scenarios, especially when frequent key updates are required.

### Key Features of the Dynamic White-box Scheme

1. **Dynamic Key Updates:**  
   In the dynamic white-box solution, a new white-box key is used as an input to the encryption key table (TK), enabling key updates without reconfiguring the entire encryption program. This allows seamless integration of the new white-box key for the key-addition step and subsequent round operations.

2. **Efficiency in Frequent Key Updates:**  
   For scenarios where keys need to be updated frequently, the dynamic white-box approach eliminates the need for regenerating lookup tables and affine transformations as required in traditional static white-box schemes. Instead, only the new white-box key is generated, allowing for rapid encryption updates.

3. **Reduced Storage Requirements:**  
   With a key length of 32 bits, the dynamic white-box scheme updates only the TK input for each key update. For 32 encryption rounds, this requires just `32 bits × 32 = 128 bytes`, significantly reducing the memory and computational overhead compared to traditional static white-box SM4 solutions.

## Advantages

- **Fast key updates:** Optimized for scenarios that demand frequent key changes.
- **Lightweight design:** Requires minimal memory updates (128 bytes per key update).
- **Dynamic encryption:** Ensures secure, real-time adaptability to new encryption keys.

Explore the repository to learn more about the implementations and how they can be applied to your encryption needs.
