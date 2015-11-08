# Glicko-2
A C++ implementation of the Glicko-2 player rating system. Glicko and Glicko-2 are alternatives to traditional Elo rating systems. Glicko was developed by Mark Glickman; the theoretical details of the system is described on [Dr. Glickman's website](http://www.glicko.net/glicko.html). Both the original Glicko and Glicko-2 rating systems are in the public domain.

# Usage
The rating system has two components: the configuration and rating objects. The constants in `glicko/config.hpp` described all of the configurable components; this includes the default rating values and system constants. Details of how to configure the rating system are described in the [Glicko-2 paper](http://www.glicko.net/glicko/glicko2.pdf).

New ratings can be created as follows:

```
    // Create a new Rating instance with the default rating and rating devation
    Glicko::Rating rating;

    // Create a new Rating instance with a specific rating
    Glicko::Rating rating(1500);

    // Create a new Rating instance with a specific rating and deviation
    Glicko::Rating rating(1500, 200);

    // Create a new Rating instance with a specific rating, deviation and volatility
    Glicko::Rating rating(1500, 310, 0.04)
```

In the Glicko rating system, player ratings are updated in batches. As described in the [Glicko-2 paper](http://www.glicko.net/glicko/glicko2.pdf), the ideal number of games in a rating period is 10-15. As such, the primary method for updating a rating is the `Glicko::Rating::Update` method. This arugments to the `Update` method are a game count, an array of opponent ratings and the outcome of the games. As per the Glicko-2 paper, a victor has the score `1.0`, a draw is scored as `0.5` and a loss is `0.0`.

For example:

```
    // The test player
    Glicko::Rating player(1500, 200);

    // The three opponents
    Glicko::Rating opponent1(1400, 30);
    Glicko::Rating opponent2(1550, 100);
    Glicko::Rating opponent3(1700, 300);

    // Update the ratings from the 3 games
    Glicko::Rating opponents[3] = {opponent1, opponent2, opponent3};
    double outcomes[3] = {1.0, 0.0, 0.0};
    player.Update(3, opponents, outcomes);
    player.Apply();
```

A version of `Update` that takes a single opponent and game outcome can also be used. This method is useful in situations where the ratings are updated with each game, such as an online chess environment:

```
    // Player1's rating
    Glicko::Rating player1(1500, 200);

    // Player2's rating
    Glicko::Rating player2(1400, 30);

    // Update the ratings based on a game in which player1 beat player2
    player1.Update(player2, 1.0);
    player2.Update(player1, 0.0);

    player1.Apply();
    player2.Apply();
```

If a player does not play any games during a rating period, the Glicko-2 document recommends that their rating deviation is updated based on the volatility, as described in Step 6 of the Glicko pdf. This can be achieved using the `Decay` function:

```
    player.Decay()
```

After calling `Update` or `Decay`, the changes must be applied using the `Glicko::Rating::Apply` method. This is neccessary because updates to multiple `Rating` instances may depend on each other and ratings should be not updated until all outcomes have been processed.

```
    player.Apply();
```

# Todo
The following features are on my todo list, and will be implemented when I have time.
* Example usage of the rating system, such as chess-like applications and team based application
* Configuration of floating point types at compiler time, e.g. prefering `float` over `double`
* Better std library conformance, e.g an overload for `Update` that uses `std::vector`

# License
This implementation of the Glicko-2 rating system is distributed under [The MIT License](https://opensource.org/licenses/MIT).